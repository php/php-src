
#include <string.h>
#include <errno.h>
#include <unistd.h>


/* Based on standard ANSI C mktemp() for NetWare */
char* mktemp(char* templateStr)
{
	char* pXs = NULL;
	char numBuf[50] = {'\0'};
	int count = 0;
	char* pPid = NULL;

	char termChar = '\0';
	char letter = 'a';
	char letter1 = 'a';

	if (templateStr && (pXs = strstr(templateStr, "XXXXXX")))
	{
		/* Generate temp name */
		termChar = pXs[6];
		ltoa(NXThreadGetId(), numBuf, 16);
		numBuf[strlen(numBuf)-1] = '\0';

        /*
		Beware! thread IDs are 8 hex digits on NW 4.11 and only the
		lower digits seem to change, whereas on NW 5 they are in the
		range of < 1000 hex or 3 hex digits in length. So the following
		logic ensures we use the least significant portion of the number.\
		*/
		if (strlen(numBuf) > 5)
			pPid = &numBuf[strlen(numBuf)-5];
		else
			pPid = numBuf;

		/*
		Temporary files, as the name suggests, are temporarily used and then
		cleaned up after usage. In the case of complex scripts, new temp files
		may be created before the old ones are deleted. So, we need to have
		a provision to create many temp files. It is found that provision for
		26 files may not be enough in such cases. Hence the logic below.

		The logic below allows 26 files (like, pla00015.tmp through plz00015.tmp)
		plus 6x26=676 (like, plaa0015.tmp through plzz0015.tmp)
		*/
		letter = 'a';
		do
		{
			sprintf(pXs, (char *)"%c%05.5s", letter, pPid);
			pXs[6] = termChar;

			if (access(templateStr, 0) != 0)
				return templateStr;		/* File does not exist */

			letter++;
		} while (letter <= 'z');

		letter1 = 'a';
		do
		{
			letter = 'a';

			do
			{
				sprintf(pXs, (char *)"%c%c%04.5s", letter1, letter, pPid);
				pXs[6] = termChar;

				if (access(templateStr, 0) != 0)
					return templateStr;		/* File does not exist */

				letter++;
			} while (letter <= 'z');

			letter1++;
		} while (letter1 <= 'z');

		errno = ENOENT;
		return NULL;
	}
	else
	{
		errno = EINVAL;
		return NULL;
	}
}

