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

// These are things that go out in the Response Header
//
#define HTTP_VER     "HTTP/1.0"
#define SERVER_VERSION "Http-Srv-Beta2/1.0"

//
// Simple wrappers for the heap APIS
//
#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(s)   HeapFree(GetProcessHeap(),0,(s))

//
// The mandatory exports from the ISAPI DLL
//

typedef struct _TIsapiContext {
	HANDLE in;
	HANDLE out;
	DWORD tid;
} TIsapiContext;

//
// Prototypes of the functions this sample implements
//
extern "C" {
HINSTANCE hDll;
typedef BOOL (WINAPI *VersionProc)(HSE_VERSION_INFO *) ;
typedef DWORD (WINAPI *HttpExtProc)(EXTENSION_CONTROL_BLOCK *);
BOOL WINAPI FillExtensionControlBlock(EXTENSION_CONTROL_BLOCK *, TIsapiContext *) ;
BOOL WINAPI GetServerVariable(HCONN, LPSTR, LPVOID, LPDWORD );
BOOL WINAPI ReadClient(HCONN,LPVOID,LPDWORD);
BOOL WINAPI WriteClient(HCONN,LPVOID,LPDWORD,DWORD);
BOOL WINAPI ServerSupportFunction(HCONN,DWORD,LPVOID,LPDWORD,LPDWORD);
VersionProc IsapiGetExtensionVersion;
HttpExtProc IsapiHttpExtensionProc;
HSE_VERSION_INFO version_info;
}

char * MakeDateStr(VOID);
char * GetEnv(char *);


#define NUM_THREADS 10
#define ITERATIONS 1
HANDLE terminate[NUM_THREADS];
HANDLE StartNow;
// quick and dirty environment
CMapStringToString IsapiEnvironment;
CStringArray IsapiFileList;
CStringArray IsapiArgList;


DWORD CALLBACK IsapiThread(void *);
int stress_main(const char *filename, const char *arg);



int main(int argc, char* argv[]) {
	LPVOID lpMsgBuf;
	char *filelist, *environment;

	if (argc < 2) {
		printf("Usage: stress filelist.txt env.txt\r\n");
		return 0;
	}
	filelist = argv[1];
	environment = argv[2];

	hDll = LoadLibrary("php4isapi.dll"); // Load our DLL

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
		fprintf(stderr,"Error: Dll 'php4isapi.dll' not found -%d\n%s\n", GetLastError(),lpMsgBuf);
		LocalFree( lpMsgBuf );
		return -1;
	}

	//
	// Find the exported functions

	IsapiGetExtensionVersion = (VersionProc)GetProcAddress(hDll,"GetExtensionVersion");
	if (!IsapiGetExtensionVersion) {
		fprintf(stderr,"Can't Get Extension Version %d\n",GetLastError());
		return -1;
	}
	IsapiHttpExtensionProc = (HttpExtProc)GetProcAddress(hDll,"HttpExtensionProc");
	if (!IsapiHttpExtensionProc) {
		fprintf(stderr,"Can't Get Extension proc %d\n",GetLastError());
		return -1;
	}

	// This should really check if the version information matches what we
	// expect.
	//
	__try {
		if (!IsapiGetExtensionVersion(&version_info) ) {
			fprintf(stderr,"Fatal: GetExtensionVersion failed\n");
			return -1;
		}
	}
	__except(1) {
		return -1;
	}

	// read config files
	FILE *fp = fopen(filelist, "r");
	if (!fp) {
		printf("Unable to open %s\r\n", filelist);
	}
	char line[2048];
	int i=0;
	while (fgets(line,sizeof(line)-1,fp)) {
		// file.php arg1 arg2 etc.
		char *p = strchr(line, ' ');
		if (p) {
			*p = 0;
			// get file
			IsapiFileList.Add(line);
			IsapiArgList.Add(p+1);
		} else {
			// just a filename is all
			IsapiFileList.Add(line);
			IsapiArgList.Add("");
		}
		i++;
	}
	fclose(fp);

	if (environment) {
	fp = fopen(environment, "r");
	i=0;
	if (fp) {
		char line[2048];
		while (fgets(line,sizeof(line)-1,fp)) {
			// file.php arg1 arg2 etc.
			char *p = strchr(line, '=');
			if (p) {
				*p=0;
				IsapiEnvironment[line]=p+1;
			}
		}
		fclose(fp);
	}
	}

	printf("Starting Threads...\r\n");
	// loop creating threads
	for (i=0; i< NUM_THREADS; i++) {
		terminate[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		DWORD tid;
		if (CreateThread(NULL, 0, IsapiThread, &terminate[i], 0, &tid)==NULL){
			SetEvent(terminate[i]);
		}
	}
	// wait for threads to finish
	WaitForMultipleObjects(NUM_THREADS, terminate, TRUE, INFINITE);

	// cleanup

	// We should really free memory (e.g., from GetEnv), but we'll be dead
	// soon enough

	FreeLibrary(hDll);
	return 0;
}


DWORD CALLBACK IsapiThread(void *p)
{
	HANDLE *terminate = (HANDLE *)p;
	DWORD filecount = IsapiFileList.GetSize();
	for (DWORD j=0; j<ITERATIONS; j++) {
		for (DWORD i=0; i<filecount; i++) {
			// execute each file
			printf("Thread %d File %s\r\n", GetCurrentThreadId(), IsapiFileList.GetAt(i));
			stress_main(IsapiFileList.GetAt(i), IsapiArgList.GetAt(i));
			Sleep(1);
		}
	}
	SetEvent(*terminate);
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
int stress_main(const char *filename, const char *arg) {

	EXTENSION_CONTROL_BLOCK ECB;
	DWORD rc;
	TIsapiContext context;

	// open output and input files
	context.tid = GetCurrentThreadId();
	CString fname;
	fname.Format("%08X.out", context.tid);
	context.out = CreateFile(fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (context.out==INVALID_HANDLE_VALUE) {
		printf("failed to open output file %s\n", fname);
		return 0;
	}
	if (arg) context.in  = CreateFile(arg, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	else context.in = INVALID_HANDLE_VALUE;
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

    ECB.lpszPathTranslated = strdup(filename);

	// Call the DLL
	//
	rc = IsapiHttpExtensionProc(&ECB);

	free (ECB.lpszPathTranslated);

	if (context.out) CloseHandle(context.out);
	if (context.in) CloseHandle(context.in);

	//if (rc == HSE_STATUS_PENDING) // We will exit in ServerSupportFunction
	//	Sleep(INFINITE);

	return 0;
		
}
//
// GetServerVariable() is how the DLL calls the main program to figure out
// the environment variables it needs. This is a required function.
//
BOOL WINAPI GetServerVariable(HCONN hConn, LPSTR lpszVariableName,
								LPVOID lpBuffer, LPDWORD lpdwSize){

	DWORD rc;
	CString value;

	if (IsapiEnvironment.Lookup(lpszVariableName, value)) {
		rc = value.GetLength();
		strncpy((char *)lpBuffer, value, *lpdwSize-1);
	} else
		rc = GetEnvironmentVariable(lpszVariableName,(char *)lpBuffer,*lpdwSize) ;

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
	if (c->in) return ReadFile(c->in,lpBuffer,(*lpdwSize), lpdwSize,NULL);
	return FALSE;
}
//
// ditto for WriteClient()
//
BOOL WINAPI WriteClient(HCONN hConn, LPVOID lpBuffer, LPDWORD lpdwSize,
			DWORD dwReserved) {
	TIsapiContext *c = (TIsapiContext *)hConn;
	if (!c) return FALSE;
	if (c->out) return WriteFile(c->out,lpBuffer,*lpdwSize, lpdwSize,NULL);
	return FALSE;
}
//
// This is a special callback function used by the DLL for certain extra 
// functionality. Look at the API help for details.
//
BOOL WINAPI ServerSupportFunction(HCONN hConn, DWORD dwHSERequest,
				LPVOID lpvBuffer, LPDWORD lpdwSize, LPDWORD lpdwDataType){

	char *lpszRespBuf;
	char * temp = NULL;
	DWORD dwBytes;
	BOOL bRet;

	switch(dwHSERequest) {
		case (HSE_REQ_SEND_RESPONSE_HEADER) :
			lpszRespBuf = (char *)xmalloc(*lpdwSize);//+ 80);//accomodate our header
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
			bRet = WriteClient(0,lpszRespBuf,&dwBytes,0);
			xfree(lpszRespBuf);

			break;
			//
			// A real server would do cleanup here
		case (HSE_REQ_DONE_WITH_SESSION):
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
			bRet = WriteClient(0,lpszRespBuf,&dwBytes,0);
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

	wsprintf(szDate,"%s, %d %s %d %d:%d.%d",DaysofWeek[systime.wDayOfWeek],
									  systime.wDay,
									  Months[systime.wMonth],
									  systime.wYear,
									  systime.wHour,systime.wMinute,
									  systime.wSecond );

	return szDate;
}
//
// Fill the ECB up 
//
BOOL WINAPI FillExtensionControlBlock(EXTENSION_CONTROL_BLOCK *ECB, TIsapiContext *context) {

	char * temp;
	ECB->cbSize = sizeof(EXTENSION_CONTROL_BLOCK);
	ECB->dwVersion = MAKELONG(HSE_VERSION_MINOR,HSE_VERSION_MAJOR);
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
char * GetEnv(LPSTR lpszEnvVar) {
	
	char *var,dummy;
	DWORD dwLen;

	if (!lpszEnvVar)
		return "";
	
	dwLen =GetEnvironmentVariable(lpszEnvVar,&dummy,1);

	if (dwLen == 0)
		return "";
	
	var = (char *)xmalloc(dwLen);
	if (!var)
		return "";
	(void)GetEnvironmentVariable(lpszEnvVar,var,dwLen);

	return var;
}
