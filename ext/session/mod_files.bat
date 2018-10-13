@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

IF _%1_==_AUTO_ (
	GOTO MakeDirs
)

IF _%2_==__ (
	ECHO Usage %0 ^<basedir^> ^<depth^> ^[^hash_bits^]
	ECHO.
	ECHO Where ^<basedir^>   is the session directory 
	ECHO       ^<depth^>     is the number of levels defined in session.save_path
	ECHO       ^[hash_bits^] is the number of bits defined in session.hash_bits_per_character
	EXIT /B 1
)

SET /A Depth=%2 + 0 2>NUL
IF /I %ERRORLEVEL% EQU 9167 GOTO DepthError
IF _%Depth%_==__ GOTO DepthError
IF /I %Depth% LEQ 0 GOTO DepthError

IF _%3_==__ GOTO DefaultBits

SET /A Bits=%3 + 0 2>NUL
IF /I %ERRORLEVEL% EQU 9167 GOTO BitsError
IF _%Bits%_==__ GOTO BitsError
IF /I %Bits% LSS 4 GOTO BitsError
IF /I %Bits% GTR 6 GOTO BitsError
GOTO BitsSet

:DefaultBits
SET Bits=4
:BitsSet

SET HashChars=0 1 2 3 4 5 6 7 8 9 A B C D E F
IF /I %Bits% GEQ 5 SET HashChars=!HashChars! G H I J K L M N O P Q R S T U V
IF /I %Bits% GEQ 6 SET HashChars=!HashChars! W X Y Z  - ,

FOR %%A IN (%HashChars%) DO (
	ECHO Making %%A
	CALL "%~0" AUTO "%~1\%%~A" %Depth%
)
GOTO :EOF

:MakeDirs
MKDIR "%~2"
SET /A ThisDepth=%3 - 1
IF /I %ThisDepth% GTR 0 FOR %%A IN (%HashChars%) DO CALL "%~0" AUTO "%~2\%%~A" %ThisDepth%
GOTO :EOF

:DepthError
ECHO ERROR: Invalid depth : %2
EXIT /B 0

:BitsError
ECHO ERROR: Invalid hash_bits : %3
EXIT /B 0
