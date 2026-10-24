/*
 * ======================================================================= *
 * File: stress .c                                                         *
 * stress tester for isapi dll's                                           *
 * based on cgiwrap                                                        *
 * ======================================================================= *
 *
*/
#define WIN32_LEAN_AND_MEAN
#include <afx.h>
#include <afxtempl.h>
#include <winbase.h>
#include <winerror.h>
#include <httpext.h>
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"

// These are things that go out in the Response Header
//
#define HTTP_VER     "HTTP/1.0"
#define SERVER_VERSION "Http-Srv-Beta2/1.0"

//
// Simple wrappers for the heap APIS
//
#define xmalloc(s) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (s))
#define xfree(s)   HeapFree(GetProcessHeap(), 0, (s))

//
// The mandatory exports from the ISAPI DLL
//
DWORD numThreads = 1;
DWORD iterations = 1;

HANDLE StartNow;
// quick and dirty environment
typedef CMapStringToString TEnvironment;
TEnvironment IsapiEnvironment;

typedef struct _TResults {
	LONG ok;
	LONG bad;
} TResults;

CStringArray IsapiFileList;  // list of filenames
CStringArray TestNames;      // --TEST--
CStringArray IsapiGetData;   // --GET--
CStringArray IsapiPostData;  // --POST--
CStringArray IsapiMatchData; // --EXPECT--
CArray<TResults, TResults> Results;

typedef struct _TIsapiContext {
	HANDLE in;
	HANDLE out;
	DWORD tid;
	TEnvironment env;
	HANDLE waitEvent;
} TIsapiContext;

//
// Prototypes of the functions this sample implements
//
extern "C" {
HINSTANCE hDll;
typedef BOOL (WINAPI *VersionProc)(HSE_VERSION_INFO *) ;
typedef DWORD (WINAPI *HttpExtProc)(EXTENSION_CONTROL_BLOCK *);
typedef BOOL (WINAPI *TerminateProc) (DWORD);
BOOL WINAPI FillExtensionControlBlock(EXTENSION_CONTROL_BLOCK *, TIsapiContext *) ;
BOOL WINAPI GetServerVariable(HCONN, LPSTR, LPVOID, LPDWORD );
BOOL WINAPI ReadClient(HCONN, LPVOID, LPDWORD);
BOOL WINAPI WriteClient(HCONN, LPVOID, LPDWORD, DWORD);
BOOL WINAPI ServerSupportFunction(HCONN, DWORD, LPVOID, LPDWORD, LPDWORD);
VersionProc IsapiGetExtensionVersion;
HttpExtProc IsapiHttpExtensionProc;
TerminateProc TerminateExtensionProc;
HSE_VERSION_INFO version_info;
}

char * MakeDateStr(VOID);
char * GetEnv(char *);




DWORD CALLBACK IsapiThread(void *);
int stress_main(const char *filename,
				const char *arg,
				const char *postfile,
				const char *matchdata);



BOOL bUseTestFiles = FALSE;
char temppath[MAX_PATH];

void stripcrlf(char *line)
{
	DWORD l = strlen(line)-1;
	if (line[l]==10 || line[l]==13) line[l]=0;
	l = strlen(line)-1;
	if (line[l]==10 || line[l]==13) line[l]=0;
}

#define COMPARE_BUF_SIZE	1024

BOOL CompareFiles(const char*f1, const char*f2)
{
	FILE *fp1, *fp2;
	bool retval;
	char buf1[COMPARE_BUF_SIZE], buf2[COMPARE_BUF_SIZE];
	int length1, length2;

	if ((fp1=fopen(f1, "r"))==NULL) {
		return FALSE;
	}

	if ((fp2=fopen(f2, "r"))==NULL) {
		fclose(fp1);
		return FALSE;
	}

	retval = TRUE; // success oriented
	while (true) {
		length1 = fread(buf1, 1, sizeof(buf1), fp1);
		length2 = fread(buf2, 1, sizeof(buf2), fp2);

		// check for end of file
		if (feof(fp1)) {
			if (!feof(fp2)) {
				retval = FALSE;
			}
			break;
		} else if (feof(fp2)) {
			if (!feof(fp1)) {
				retval = FALSE;
			}
			break;
		}

		// compare data
		if (length1!=length2
			|| memcmp(buf1, buf2, length1)!=0) {
			retval = FALSE;
			break;
		}
	}
	fclose(fp1);
	fclose(fp2);

	return retval;
}


BOOL CompareStringWithFile(const char *filename, const char *str, unsigned int str_length)
{
	FILE *fp;
	bool retval;
	char buf[COMPARE_BUF_SIZE];
	unsigned int offset=0, readbytes;
	fprintf(stderr, "test %s\n",filename);
	if ((fp=fopen(filename, "rb"))==NULL) {
		fprintf(stderr, "Error opening %s\n",filename);
		return FALSE;
	}

	retval = TRUE; // success oriented
	while (true) {
		readbytes = fread(buf, 1, sizeof(buf), fp);

		// check for end of file

		if (offset+readbytes > str_length
			|| memcmp(buf, str+offset, readbytes)!=NULL) {
			fprintf(stderr, "File missmatch %s\n",filename);
			retval = FALSE;
			break;
		}
		if (feof(fp)) {
			if (!retval) fprintf(stderr, "File zero length %s\n",filename);
			break;
		}
	}
	fclose(fp);

	return retval;
}


BOOL ReadGlobalEnvironment(const char *environment)
{
	if (environment) {
	FILE *fp = fopen(environment, "r");
	DWORD i=0;
	if (fp) {
		char line[2048];
		while (fgets(line, sizeof(line)-1, fp)) {
			// file.php arg1 arg2 etc.
			char *p = strchr(line, '=');
			if (p) {
				*p=0;
				IsapiEnvironment[line]=p+1;
			}
		}
		fclose(fp);
		return IsapiEnvironment.GetCount() > 0;
	}
	}
	return FALSE;
}

BOOL ReadFileList(const char *filelist)
{
	FILE *fp = fopen(filelist, "r");
	if (!fp) {
		printf("Unable to open %s\r\n", filelist);
	}
	char line[2048];
	int i=0;
	while (fgets(line, sizeof(line)-1, fp)) {
		// file.php arg1 arg2 etc.
		stripcrlf(line);
		if (strlen(line)>3) {
			char *p = strchr(line, ' ');
			if (p) {
				*p = 0;
				// get file

				IsapiFileList.Add(line);
				IsapiGetData.Add(p+1);
			} else {
				// just a filename is all
				IsapiFileList.Add(line);
				IsapiGetData.Add("");
			}
		}

		// future use
		IsapiPostData.Add("");
		IsapiMatchData.Add("");
		TestNames.Add("");

		i++;
	}
	Results.SetSize(TestNames.GetSize());

	fclose(fp);
	return IsapiFileList.GetSize() > 0;
}

void DoThreads() {

	if (IsapiFileList.GetSize() == 0) {
		printf("No Files to test\n");
		return;
	}

	printf("Starting Threads...\n");
	// loop creating threads
	DWORD tid;
	HANDLE *threads = new HANDLE[numThreads];
	DWORD i;
	for (i=0; i< numThreads; i++) {
		threads[i]=CreateThread(NULL, 0, IsapiThread, NULL, CREATE_SUSPENDED, &tid);
	}
	for (i=0; i< numThreads; i++) {
		if (threads[i]) ResumeThread(threads[i]);
	}
	// wait for threads to finish
	WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);
	for (i=0; i< numThreads; i++) {
		CloseHandle(threads[i]);
	}
	delete [] threads;
}

void DoFileList(const char *filelist, const char *environment)
{
	// read config files

	if (!ReadFileList(filelist)) {
		printf("No Files to test!\r\n");
		return;
	}

	ReadGlobalEnvironment(environment);

	DoThreads();
}


/**
 * ParseTestFile
 * parse a single phpt file and add it to the arrays
 */
BOOL ParseTestFile(const char *path, const char *fn)
{
	// parse the test file
	char filename[MAX_PATH];
	_snprintf(filename, sizeof(filename)-1, "%s\\%s", path, fn);
	char line[1024];
	memset(line, 0, sizeof(line));
	CString cTest, cSkipIf, cPost, cGet, cFile, cExpect;
	printf("Reading %s\r\n", filename);

	enum state {none, test, skipif, post, get, file, expect} parsestate = none;

	FILE *fp = fopen(filename, "rb");
	char *tn = _tempnam(temppath,"pht.");
	char *en = _tempnam(temppath,"exp.");
	FILE *ft = fopen(tn, "wb+");
	FILE *fe = fopen(en, "wb+");
	if (fp && ft && fe) {
		while (fgets(line, sizeof(line)-1, fp)) {
			if (line[0]=='-') {
				if (_strnicmp(line, "--TEST--", 8)==0) {
					parsestate = test;
					continue;
				} else if (_strnicmp(line, "--SKIPIF--", 10)==0) {
					parsestate = skipif;
					continue;
				} else if (_strnicmp(line, "--POST--", 8)==0) {
					parsestate = post;
					continue;
				} else if (_strnicmp(line, "--GET--", 7)==0) {
					parsestate = get;
					continue;
				} else if (_strnicmp(line, "--FILE--", 8)==0) {
					parsestate = file;
					continue;
				} else if (_strnicmp(line, "--EXPECT--", 10)==0) {
					parsestate = expect;
					continue;
				}
			}
			switch (parsestate) {
			case test:
				stripcrlf(line);
				cTest = line;
				break;
			case skipif:
				cSkipIf += line;
				break;
			case post:
				cPost += line;
				break;
			case get:
				cGet += line;
				break;
			case file:
				fputs(line, ft);
				break;
			case expect:
				fputs(line, fe);
				break;
			}
		}

		fclose(fp);
		fclose(ft);
		fclose(fe);

		if (!cTest.IsEmpty()) {
			IsapiFileList.Add(tn);
			TestNames.Add(cTest);
			IsapiGetData.Add(cGet);
			IsapiPostData.Add(cPost);
			IsapiMatchData.Add(en);
			free(tn);
			free(en);
			return TRUE;
		}
	}
	free(tn);
	free(en);
	return FALSE;
}


/**
 * GetTestFiles
 * Recurse through the path and subdirectories, parse each phpt file
 */
BOOL GetTestFiles(const char *path)
{
	// find all files .phpt under testpath\tests
	char FindPath[MAX_PATH];
	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(WIN32_FIND_DATA));

	_snprintf(FindPath, sizeof(FindPath)-1, "%s\\*.*", path);
	HANDLE fh = FindFirstFile(FindPath, &fd);
	if (fh != INVALID_HANDLE_VALUE) {
		do {
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!strchr(fd.cFileName, '.')) {
				// subdirectory, recurse into it
				char NewFindPath[MAX_PATH];
				_snprintf(NewFindPath, sizeof(NewFindPath)-1, "%s\\%s", path, fd.cFileName);
				GetTestFiles(NewFindPath);
			} else if (strstr(fd.cFileName, ".phpt")) {
				// got test file, parse it now
				if (ParseTestFile(path, fd.cFileName)) {
					printf("Test File Added: %s\\%s\r\n", path, fd.cFileName);
				}
			}
			memset(&fd, 0, sizeof(WIN32_FIND_DATA));
		} while (FindNextFile(fh, &fd) != 0);
		FindClose(fh);
	}
	return IsapiFileList.GetSize() > 0;
}

void DeleteTempFiles(const char *mask)
{
	char FindPath[MAX_PATH];
	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(WIN32_FIND_DATA));

	_snprintf(FindPath, sizeof(FindPath)-1, "%s\\%s", temppath, mask);
	HANDLE fh = FindFirstFile(FindPath, &fd);
	if (fh != INVALID_HANDLE_VALUE) {
		do {
			char NewFindPath[MAX_PATH];
			_snprintf(NewFindPath, sizeof(NewFindPath)-1, "%s\\%s", temppath, fd.cFileName);
			DeleteFile(NewFindPath);
			memset(&fd, 0, sizeof(WIN32_FIND_DATA));
		} while (FindNextFile(fh, &fd) != 0);
		FindClose(fh);
	}
}

void DoTestFiles(const char *filelist, const char *environment)
{
	if (!GetTestFiles(filelist)) {
		printf("No Files to test!\r\n");
		return;
	}

	Results.SetSize(IsapiFileList.GetSize());

	ReadGlobalEnvironment(environment);

	DoThreads();

	printf("\r\nRESULTS:\r\n");
	// show results:
	DWORD r = Results.GetSize();
	for (DWORD i=0; i< r; i++) {
		TResults result = Results.GetAt(i);
		printf("%s\r\nOK: %d FAILED: %d\r\n", TestNames.GetAt(i), result.ok, result.bad);
	}

	// delete temp files
	printf("Deleting Temp Files\r\n");
	DeleteTempFiles("exp.*");
	DeleteTempFiles("pht.*");
	printf("Done\r\n");
}

#define OPTSTRING "m:f:d:h:t:i:"
static void _usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "stresstest";
	}

	printf("Usage: %s -m <isapi.dll> -d|-l <file> [-t <numthreads>] [-i <numiterations>]\n"
				"  -m             path to isapi dll\n"
				"  -d <directory> php directory (to run php test files).\n"
				"  -f <file>      file containing list of files to run\n"
				"  -t             number of threads to use (default=1)\n"
                "  -i             number of iterations per thread (default=1)\n"
				"  -h             This help\n", prog);
}
int main(int argc, char* argv[])
{
	LPVOID lpMsgBuf;
	char *filelist=NULL, *environment=NULL, *module=NULL;
	int c = NULL;
	while ((c=ap_getopt(argc, argv, OPTSTRING))!=-1) {
		switch (c) {
			case 'd':
				bUseTestFiles = TRUE;
				filelist = strdup(ap_optarg);
				break;
			case 'f':
				bUseTestFiles = FALSE;
				filelist = strdup(ap_optarg);
				break;
			case 'e':
				environment = strdup(ap_optarg);
				break;
			case 't':
				numThreads = atoi(ap_optarg);
				break;
			case 'i':
				iterations = atoi(ap_optarg);
				break;
			case 'm':
				module = strdup(ap_optarg);
				break;
			case 'h':
				_usage(argv[0]);
				exit(0);
				break;
		}
	}
	if (!module || !filelist) {
		_usage(argv[0]);
		exit(0);
	}

	GetTempPath(sizeof(temppath), temppath);
	hDll = LoadLibrary(module); // Load our DLL

	if (!hDll) {
		FormatMessage(
		   FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		    NULL,
		    GetLastError(),
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		    (LPTSTR) &lpMsgBuf,
		    0,
		    NULL
		);
		fprintf(stderr,"Error: Dll 'php7isapi.dll' not found -%d\n%s\n", GetLastError(), lpMsgBuf);
		free (module);
		free(filelist);
		LocalFree( lpMsgBuf );
		return -1;
	}

	//
	// Find the exported functions

	IsapiGetExtensionVersion = (VersionProc)GetProcAddress(hDll,"GetExtensionVersion");
	if (!IsapiGetExtensionVersion) {
		fprintf(stderr,"Can't Get Extension Version %d\n", GetLastError());
		free (module);
		free(filelist);
		return -1;
	}
	IsapiHttpExtensionProc = (HttpExtProc)GetProcAddress(hDll,"HttpExtensionProc");
	if (!IsapiHttpExtensionProc) {
		fprintf(stderr,"Can't Get Extension proc %d\n", GetLastError());
		free (module);
		free(filelist);
		return -1;
	}
	TerminateExtensionProc = (TerminateProc) GetProcAddress(hDll,
                                          "TerminateExtension");

	// This should really check if the version information matches what we
	// expect.
	//
	if (!IsapiGetExtensionVersion(&version_info) ) {
		fprintf(stderr,"Fatal: GetExtensionVersion failed\n");
		free (module);
		free(filelist);
		return -1;
	}

	if (bUseTestFiles) {
		char TestPath[MAX_PATH];
		if (filelist != NULL)
			_snprintf(TestPath, sizeof(TestPath)-1, "%s\\tests", filelist);
		else strcpy(TestPath, "tests");
		DoTestFiles(TestPath, environment);
	} else {
		DoFileList(filelist, environment);
	}

	// cleanup
	if (TerminateExtensionProc) TerminateExtensionProc(0);

	// We should really free memory (e.g., from GetEnv), but we'll be dead
	// soon enough

	FreeLibrary(hDll);
	free (module);
	free(filelist);
	return 0;
}


DWORD CALLBACK IsapiThread(void *p)
{
	DWORD filecount = IsapiFileList.GetSize();

	for (DWORD j=0; j<iterations; j++) {
		for (DWORD i=0; i<filecount; i++) {
			// execute each file
			CString testname = TestNames.GetAt(i);
			BOOL ok = FALSE;
			if (stress_main(IsapiFileList.GetAt(i),
						IsapiGetData.GetAt(i),
						IsapiPostData.GetAt(i),
						IsapiMatchData.GetAt(i))) {
				InterlockedIncrement(&Results[i].ok);
				ok = TRUE;
			} else {
				InterlockedIncrement(&Results[i].bad);
				ok = FALSE;
			}

			if (testname.IsEmpty()) {
				printf("Thread %d File %s\n", GetCurrentThreadId(), IsapiFileList.GetAt(i));
			} else {
				printf("tid %d: %s %s\n", GetCurrentThreadId(), testname, ok?"OK":"FAIL");
			}
			Sleep(10);
		}
	}
	printf("Thread ending...\n");
	return 0;
}

/*
 * ======================================================================= *
 * In the startup of this program, we look at our executable name and      *
 * replace the ".EXE" with ".DLL" to find the ISAPI DLL we need to load.   *
 * This means that the executable need only be given the same "name" as    *
 * the DLL to load. There is no recompilation required.                    *
 * ======================================================================= *
*/
BOOL stress_main(const char *filename,
				const char *arg,
				const char *postdata,
				const char *matchdata)
{

	EXTENSION_CONTROL_BLOCK ECB;
	DWORD rc;
	TIsapiContext context;

	// open output and input files
	context.tid = GetCurrentThreadId();
	CString fname;
	fname.Format("%08X.out", context.tid);

	context.out = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
	if (context.out==INVALID_HANDLE_VALUE) {
		printf("failed to open output file %s\n", fname);
		return 0;
	}

	// not using post files
	context.in = INVALID_HANDLE_VALUE;

	//
	// Fill the ECB with the necessary information
	//
	if (!FillExtensionControlBlock(&ECB, &context) ) {
		fprintf(stderr,"Fill Ext Block Failed\n");
		return -1;
	}

	// check for command line argument,
	// first arg = filename
	// this is added for testing php from command line

	context.env.RemoveAll();
	context.env["PATH_TRANSLATED"]= filename;
	context.env["SCRIPT_MAP"]= filename;
	context.env["CONTENT_TYPE"]= "";
	context.env["CONTENT_LENGTH"]= "";
	context.env["QUERY_STRING"]= arg;
	context.env["METHOD"]="GET";
	context.env["PATH_INFO"] = "";
	context.waitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	char buf[MAX_PATH];
	if (postdata && *postdata !=0) {
		ECB.cbAvailable = strlen(postdata);
		ECB.cbTotalBytes = ECB.cbAvailable;
		ECB.lpbData = (unsigned char *)postdata;
		context.env["METHOD"]="POST";

		_snprintf(buf, sizeof(buf)-1, "%d", ECB.cbTotalBytes);
		context.env["CONTENT_LENGTH"]=buf;

		context.env["CONTENT_TYPE"]="application/x-www-form-urlencoded";
	}
	ECB.lpszMethod = strdup(context.env["METHOD"]);
    ECB.lpszPathTranslated = strdup(filename);
	ECB.lpszQueryString = strdup(arg);
	ECB.lpszPathInfo = strdup(context.env["PATH_INFO"]);


	// Call the DLL
	//
	rc = IsapiHttpExtensionProc(&ECB);
	if (rc == HSE_STATUS_PENDING) {
		// We will exit in ServerSupportFunction
		WaitForSingleObject(context.waitEvent, INFINITE);
	}
	CloseHandle(context.waitEvent);
	//Sleep(75);
	free(ECB.lpszPathTranslated);
	free(ECB.lpszQueryString);
	free(ECB.lpszMethod);
	free(ECB.lpszPathInfo);

	BOOL ok = TRUE;

	if (context.out != INVALID_HANDLE_VALUE) CloseHandle(context.out);

	// compare the output with the EXPECT section
	if (matchdata && *matchdata != 0) {
		ok = CompareFiles(fname, matchdata);
	}

	DeleteFile(fname);

	return ok;

}
//
// GetServerVariable() is how the DLL calls the main program to figure out
// the environment variables it needs. This is a required function.
//
BOOL WINAPI GetServerVariable(HCONN hConn, LPSTR lpszVariableName,
								LPVOID lpBuffer, LPDWORD lpdwSize){

	DWORD rc;
	CString value;
	TIsapiContext *c = (TIsapiContext *)hConn;
	if (!c) return FALSE;

	if (IsapiEnvironment.Lookup(lpszVariableName, value)) {
		rc = value.GetLength();
		strncpy((char *)lpBuffer, value, *lpdwSize-1);
	} else if (c->env.Lookup(lpszVariableName, value)) {
		rc = value.GetLength();
		strncpy((char *)lpBuffer, value, *lpdwSize-1);
	} else
		rc = GetEnvironmentVariable(lpszVariableName, (char *)lpBuffer, *lpdwSize) ;

	if (!rc) { // return of 0 indicates the variable was not found
		SetLastError(ERROR_NO_DATA);
		return FALSE;
	}

	if (rc > *lpdwSize) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return FALSE;
	}

	*lpdwSize =rc + 1 ; // GetEnvironmentVariable does not count the NULL

	return TRUE;

}
//
// Again, we don't have an HCONN, so we simply wrap ReadClient() to
// ReadFile on stdin. The semantics of the two functions are the same
//
BOOL WINAPI ReadClient(HCONN hConn, LPVOID lpBuffer, LPDWORD lpdwSize) {
	TIsapiContext *c = (TIsapiContext *)hConn;
	if (!c) return FALSE;

	if (c->in != INVALID_HANDLE_VALUE)
		return ReadFile(c->in, lpBuffer, (*lpdwSize), lpdwSize, NULL);

	return FALSE;
}
//
// ditto for WriteClient()
//
BOOL WINAPI WriteClient(HCONN hConn, LPVOID lpBuffer, LPDWORD lpdwSize,
			DWORD dwReserved) {
	TIsapiContext *c = (TIsapiContext *)hConn;
	if (!c) return FALSE;

	if (c->out != INVALID_HANDLE_VALUE)
		return WriteFile(c->out, lpBuffer, *lpdwSize, lpdwSize, NULL);
	return FALSE;
}
//
// This is a special callback function used by the DLL for certain extra
// functionality. Look at the API help for details.
//
BOOL WINAPI ServerSupportFunction(HCONN hConn, DWORD dwHSERequest,
				LPVOID lpvBuffer, LPDWORD lpdwSize, LPDWORD lpdwDataType){

	TIsapiContext *c = (TIsapiContext *)hConn;
	char *lpszRespBuf;
	char * temp = NULL;
	DWORD dwBytes;
	BOOL bRet = TRUE;

	switch(dwHSERequest) {
		case (HSE_REQ_SEND_RESPONSE_HEADER) :
			lpszRespBuf = (char *)xmalloc(*lpdwSize);//+ 80);//accommodate our header
			if (!lpszRespBuf)
				return FALSE;
			wsprintf(lpszRespBuf,"%s",
				//HTTP_VER,

				/* Default response is 200 Ok */

				//lpvBuffer?lpvBuffer:"200 Ok",

				/* Create a string for the time. */
				//temp=MakeDateStr(),

				//SERVER_VERSION,

				/* If this exists, it is a pointer to a data buffer to
				   be sent. */
				lpdwDataType?(char *)lpdwDataType:NULL);

			if (temp) xfree(temp);

			dwBytes = strlen(lpszRespBuf);
			bRet = WriteClient(0, lpszRespBuf, &dwBytes, 0);
			xfree(lpszRespBuf);

			break;
			//
			// A real server would do cleanup here
		case (HSE_REQ_DONE_WITH_SESSION):
			SetEvent(c->waitEvent);
			//ExitThread(0);
			break;

		//
		// This sends a redirect (temporary) to the client.
		// The header construction is similar to RESPONSE_HEADER above.
		//
		case (HSE_REQ_SEND_URL_REDIRECT_RESP):
			lpszRespBuf = (char *)xmalloc(*lpdwSize +80) ;
			if (!lpszRespBuf)
				return FALSE;
			wsprintf(lpszRespBuf,"%s %s %s\r\n",
					HTTP_VER,
					"302 Moved Temporarily",
					(lpdwSize > 0)?lpvBuffer:0);
			xfree(temp);
			dwBytes = strlen(lpszRespBuf);
			bRet = WriteClient(0, lpszRespBuf, &dwBytes, 0);
			xfree(lpszRespBuf);
			break;
		default:
			return FALSE;
		break;
	}
	return bRet;

}
//
// Makes a string of the date and time from GetSystemTime().
// This is in UTC, as required by the HTTP spec.`
//
char * MakeDateStr(void){
	SYSTEMTIME systime;
	char *szDate= (char *)xmalloc(64);

	char * DaysofWeek[] = {"Sun","Mon","Tue","Wed","Thurs","Fri","Sat"};
	char * Months[] = {"NULL","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
						"Sep","Oct","Nov","Dec"};

	GetSystemTime(&systime);

	wsprintf(szDate,"%s, %d %s %d %d:%d.%d", DaysofWeek[systime.wDayOfWeek],
									  systime.wDay,
									  Months[systime.wMonth],
									  systime.wYear,
									  systime.wHour, systime.wMinute,
									  systime.wSecond );

	return szDate;
}
//
// Fill the ECB up
//
BOOL WINAPI FillExtensionControlBlock(EXTENSION_CONTROL_BLOCK *ECB, TIsapiContext *context) {

	char * temp;
	ECB->cbSize = sizeof(EXTENSION_CONTROL_BLOCK);
	ECB->dwVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);
	ECB->ConnID = (void *)context;
	//
	// Pointers to the functions the DLL will call.
	//
	ECB->GetServerVariable = GetServerVariable;
	ECB->ReadClient  = ReadClient;
	ECB->WriteClient = WriteClient;
	ECB->ServerSupportFunction = ServerSupportFunction;

	//
	// Fill in the standard CGI environment variables
	//
	ECB->lpszMethod = GetEnv("REQUEST_METHOD");
	if (!ECB->lpszMethod) ECB->lpszMethod = "GET";

	ECB->lpszQueryString = GetEnv("QUERY_STRING");
	ECB->lpszPathInfo = GetEnv("PATH_INFO");
	ECB->lpszPathTranslated = GetEnv("PATH_TRANSLATED");
	ECB->cbTotalBytes=( (temp=GetEnv("CONTENT_LENGTH")) ? (atoi(temp)): 0);
	ECB->cbAvailable = 0;
	ECB->lpbData = (unsigned char *)"";
	ECB->lpszContentType = GetEnv("CONTENT_TYPE");
	return TRUE;

}

//
// Works like _getenv(), but uses win32 functions instead.
//
char *GetEnv(LPSTR lpszEnvVar)
{

	char *var, dummy;
	DWORD dwLen;

	if (!lpszEnvVar)
		return "";

	dwLen =GetEnvironmentVariable(lpszEnvVar, &dummy, 1);

	if (dwLen == 0)
		return "";

	var = (char *)xmalloc(dwLen);
	if (!var)
		return "";
	(void)GetEnvironmentVariable(lpszEnvVar, var, dwLen);

	return var;
}
