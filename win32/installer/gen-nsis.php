; PHP Installer for NSIS
; Based on Welcome/Finish Page Example Script (Written by Joost Verburg)
; Author: Wez Furlong <wez@thebrainroom.com>
; $Id$
<?php

define('PHPVERSION', phpversion());
define('SOURCEDIR',	 dirname(__FILE__) . "\\..\\..\\");
define('DISTDIR',	 getcwd());

/* for testing, install system files in a safe dir */

if (false) {
	$SYSDIR = "\$INSTDIR\\system";
} else {
	$SYSDIR = "\$SYSDIR";
}

$sections = array(
	"core" => array(
		"label" => "-PHP Core (Required)",
		"files" => array(
			"$SYSDIR" => array(
				"dlls\\*.dll",
				"php4ts.dll",
			),
			"\$INSTDIR" => array(
				"*.txt",
				"php.gif",
				"php.ini-*",
			),
		),
		"extras" => "Call CopyPHPIni\nWriteUninstaller \"\$INSTDIR\\Uninstall.exe\""
	),
 
	"cgi" => array(
		"group" => "SAPI",
		"label" => "CGI",
		"description" => "CGI Interface - should work with most web servers",
	  	"files" => array(
	  		"\$INSTDIR" => array(
	  			"php.exe"
	  		),
			"\$INSTDIR\\sapi" => array(
				"sapi\\pws-php4cgi.reg"
			),
	  	),
	),

	/* CLI is required by the installer */
	"cli" => array(
		"group" => "SAPI",
		"label" => "-CLI",
		"description" => "Command Line Interface for running PHP scripts as batch files",
		"files" => array(
			"\$INSTDIR\\cli" => array(
				"cli\\php.exe"
			),
		),
	),

    "embed" => array(
		"group" => "SAPI",
		"label" => "Embed",
		"description" => "Libraries for embedding PHP into your own projects",
		"files" => array(
			"\$INSTDIR" => array(
				"*.lib"
			),
		),
	),

	"mibs" => array(
		"group" => "Extras",
		"label" => "MIBS for SNMP",
		"description" => "MIB information for the SNMP extension",
		"files" => array(
			"\$INSTDIR\\mibs" => array(
				"mibs\\*"
			),
		),
	),

	"pdfstuff" => array(
		"group" => "Extras",
		"label" => "PDF support files",
		"description" => "Fonts and codepage data files for the PDF extensions",
		"files" => array(
			"\$INSTDIR\\pdf-related" => array(
				"pdf-related\\*"
			),
		),
	),

	"mimemagic" => array(
		"group" => "Extras",
		"label" => "Mime Magic",
		"description" => "The magic information file for the mimemagic extension",
		"files" => array(
			"\$INSTDIR" => array(
				"magic.mime"
			),
		),
	),

	"openssl" => array(
		"group" => "Extras",
		"label" => "OpenSSL",
		"description" => "OpenSSL configuration information which you must read if you plan to use PHP as a Certificate Authority",
		"files" => array(
			"\$INSTDIR\\openssl" => array(
				"openssl\\*"
			),
		),
	),

          

);

$groups = array(
	'SAPI'	=> array("SAPI", "Select which Server API's to install."),
	'EXT'	=> array("Extensions", "Select which extensions to install."),
	'XSAPI' => array("Experimental SAPI", "Experimental Server API's - use at your own risk!"),
	'XEXT'	=> array("Experimental Extensions", "Experimental Extensions - use at your own risk!"),
	'Extras'	=> array("Extras", "Supplemental Files"),
	);

function xglob($pattern)
{
	if (strpos($pattern, '*') === false) {
		return array($pattern);
	}
	$dir = dirname($pattern);
	$pattern = basename($pattern);

	$pattern = str_replace(array('*', '.'), array('.*', '\.'), $pattern);

	$files = array();

	$d = opendir($dir);
	while ($f = readdir($d)) {
		if ($f == '.' || $f == '..' || preg_match('/^\..*\.swp$/', $f))
			continue;
		if (preg_match('@' . $pattern . '@i', $f)) {
			$files[] = $dir . '\\' . $f;
		}
	}
	
	return $files;
}

/* Look for extensions and determine their info based on the source tree.
 * We can determine the name and author from the CREDITS file.
 * We can determine experimental status from the EXPERIMENTAL file.
 * It would be nice to have a standard description line in the CREDITS too.
 */
$extension_abbreviations = array(
	'ifx' => 'informix',
	'gd2' => 'gd',
	);

$sapi_abbreviations = array(
	'apache2'	=>	'apache2filter',
	'srvlt'	=>	'servlet',
);

function add_sections($pattern, $groupname, $abbrev, &$sections, $sourcedirs, $instdir)
{
	$avail = xglob($pattern);

	foreach ($avail as $extname) {
		/* mangle name */
		if (preg_match('/(php_|php4|php)([^_].*)\.dll/', basename($extname), $matches)) {
			$ext = $matches[2];
		} else {
			$ext = basename($extname);
		}

		if (isset($abbrev[$ext])) {
			$ext = $abbrev[$ext];
		}

		$extdir = null;
		
		foreach ($sourcedirs as $sourcedir) {
			// Allow absolute path for separate PECL checkout
			if ($sourcedir{0} == '\\' || $sourcedir{1} == ':')
				$extdir = $sourcedir;
			else
				$extdir = SOURCEDIR . $sourcedir . '/' . $ext . '/';
				
			if (is_dir($extdir))
				break;

			$extdir = null;
		}

		if ($extdir === null || !is_dir($extdir)) {
			echo ";  WARNING: could not find source dir for extension $extname -> $extdir ($ext)\n";
		}

		$exp = file_exists($extdir . 'EXPERIMENTAL');
		if (file_exists($extdir . 'CREDITS')) {
			list($title, $authors, $description) = file($extdir . 'CREDITS');
			$title = trim($title);
			$authors = trim($authors);
			$description = trim($description);

			if (strlen($title) == 0)
				$title = $ext;

		} else {
			$title = $ext;
			$authors = "Unknown, or uncredited";
			$description = "";
		}

		/* build list of support files.
		 * in theory, only needed for the srvlt sapi, but useful to keep this
		 * generic */
		$filepat = preg_replace('/\.[^\.]+$/', '.*', $extname);

		if ($groupname == 'EXT') {
		}

		$sections['ext_' . $ext] = array(
				'group' => ($exp ? 'X' : '') . $groupname,
				'label' => $title,
				'description' => $title . ".\$\\nAuthors: $authors",
				'files' => array(
					"\$INSTDIR\\" . $instdir => array(
						$filepat
						),
					),
/*				'extras' => $groupname == 'EXT' ? "Push \"extension=" . basename($extname) . "\"\nCall AddIniSetting\n\n" : "" */
				);

	}


}

add_sections('extensions\\*.dll', 'EXT', $extension_abbreviations, $sections, array('ext'), 'extensions');
add_sections('sapi\\*.dll', 'SAPI', $sapi_abbreviations, $sections, array('sapi'), 'sapi');

 



$SECTIONS = "";
$sections_stage1 = array();

/* list of files to uninstall */
$uninstall = array();
$rmdirs = array();
/* description texts */
$descriptions = array();

$dirs_no_delete = array("\$SYSDIR", "\$INSTDIR");

foreach ($sections as $sectionid => $sectiondata) {
	$descriptions[] = "\t!insertmacro MUI_DESCRIPTION_TEXT \${Sec$sectionid} \"" . $sectiondata['description'] . "\"";

	if (isset($sectiondata['group'])) {
		$sub = "Sub";
		$group = $sectiondata['group'];
	} else {
		$sub = "";
		$group = '_top_';
	}

	$body = "Section \"" . $sectiondata['label'] . "\" Sec$sectionid\n";
	
	foreach ($sectiondata['files'] as $outputdir => $filelist) {
		$body .= "\tSetOutPath \"$outputdir\"\n";
		foreach ($filelist as $pattern) {
			$files = xglob($pattern);
			foreach ($files as $filename) {
				$uninstall[] = "\tDelete \"$outputdir\\" . basename($filename) . "\"";
				
				$body .= "\tFile \"$filename\"\n";
			}
		}
		if (!in_array($outputdir, $dirs_no_delete) && !isset($rmdirs[$outputdir])) {
			$rmdirs[$outputdir] = "\tRMDir \"$outputdir\"";
		}
	}

	$body .= $sectiondata['extras'];

	$body .= "\nSectionEnd\n";

	$sections_stage1[$group] .= $body . "\n\n";
}

foreach ($sections_stage1 as $group => $data)
{
	if ($group == '_top_') {
		$SECTIONS .= $data . "\n";
	} else {
		$descriptions[] = "\t!insertmacro MUI_DESCRIPTION_TEXT \${SecGroup$group} \"" . $groups[$group][1] . "\"";
		$SECTIONS .= "SubSection /e \"" . $groups[$group][0] . "\" SecGroup$group\n$data\nSubSectionEnd\n\n";
	}
}

/* Now build descriptions */
$DESCRIPTIONS = implode("\n", $descriptions) . "\n";

/* And the uninstallation section */
$UNINSTALL = implode("\n", $uninstall) . "\n" . implode("\n", $rmdirs) . "\n";

?>

!define MUI_PRODUCT "PHP - Hypertext Preprocessor"
!define MUI_VERSION "<?= PHPVERSION ?>"
; The second UI has a wider area for the component names
!define MUI_UI "${NSISDIR}\Contrib\UIs\modern2.exe"	

!include "${NSISDIR}\Contrib\Modern UI\System.nsh"

;--------------------------------
;Configuration

!define MUI_WELCOMEPAGE
!define MUI_LICENSEPAGE
!define MUI_COMPONENTSPAGE
!define MUI_DIRECTORYPAGE
  
!define MUI_FINISHPAGE
;!define MUI_FINISHPAGE_RUN "$INSTDIR\modern.exe"
  
!define MUI_ABORTWARNING
  
!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
    
;Language
!insertmacro MUI_LANGUAGE "English"

;General
OutFile "InstallPHP<?= PHPVERSION ?>.exe"

SetCompressor bzip2
ShowInstDetails show
;License page
LicenseData "license.txt"

;Folder-selection page
InstallDir "C:\PHP-<?= PHPVERSION ?>"
;define NSIS_CONFIG_LOG "install.log"
;LogSet on

!cd <?= SOURCEDIR ?>win32\installer

;Things that need to be extracted on startup (keep these lines before any File command!)
;Only useful for BZIP2 compression
;Use ReserveFile for your own Install Options ini files too!
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
!insertmacro MUI_RESERVEFILE_SPECIALINI
!insertmacro MUI_RESERVEFILE_SPECIALBITMAP

!cd <?= DISTDIR ?>

!insertmacro MUI_SYSTEM


;--------------------------------
;Installer Sections

Function AddIniSetting
	Pop $R0

	FileOpen $R2 "$INSTDIR\.ini-add" "a"
	FileSeek $R2 0 END
	FileWrite $R2 "$R0$\n"
	FileClose $R2

FunctionEnd

Function CopyPHPIni
	; Ensure that we have a working php.ini to reflect the
	; installation options.
	; Extensions will call a function to activate their entry
	; in the ini file as they are installed.

	Rename "$WINDIR\php.ini" "$WINDIR\php.ini.old"
	CopyFiles "$INSTDIR\php.ini-dist" "$WINDIR\php.ini"

; These files will be deleted during post-installation
	CopyFiles "<?= $SYSDIR ?>\php4ts.dll" "$INSTDIR\php4ts.dll"
	File "<?= dirname(__FILE__) ?>\setini.php"

; Set the extension_dir setting in the php.ini
	Push "extension_dir=$\"$INSTDIR\extensions$\""
	Call AddIniSetting

FunctionEnd

; Generated Section Info
<?= $SECTIONS ?>
; ---------

; Perform final actions after everything has been installed
Section -post
	; Merge ini settings

	Sleep 1000
	
	ExecWait "$\"$INSTDIR\cli\php.exe$\" $\"-n$\" $\"$INSTDIR\setini.php$\" $\"$WINDIR\php.ini$\" $\"$INSTDIR\.ini-add$\""

	Delete "$INSTDIR\.ini-add" ; Created by the AddIniSetting function
	Delete "$INSTDIR\setini.php"
	Delete "$INSTDIR\php4ts.dll"

	; Add to Add/Remove programs list
	WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PHP-<?= PHPVERSION ?>" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PHP-<?= PHPVERSION ?>" "DisplayName" "PHP-<?= PHPVERSION ?> (Uninstall only)"


SectionEnd

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTIONS_DESCRIPTION_BEGIN
<?= $DESCRIPTIONS ?>
!insertmacro MUI_FUNCTIONS_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
<?= $UNINSTALL ?>

	Delete "$INSTDIR\Uninstall.exe"
	Delete "$WINDIR\php.ini"
	RMDir "$INSTDIR"
	; Remove from Add/Remove programs list
	DeleteRegKey /ifempty HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PHP-<?= PHPVERSION ?>"

	!insertmacro MUI_UNFINISHHEADER

SectionEnd
; vim:sw=4:ts=4:
; vim600:sw=4:ts=4:noet:fdm=marker
