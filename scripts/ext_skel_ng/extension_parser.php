<?php
	// {{{ includes

	require_once "php_element.php";
	require_once "php_constant.php";
	require_once "php_function.php";
	require_once "php_resource.php";
	require_once "php_logo.php";

	require_once "xml_stream_parser.php";
	require_once "xml_stream_callback_parser.php";

	// }}} 

	class extension_parser extends xml_stream_callback_parser {

		// {{{ constructor

	function extension_parser($stream) {
		$this->template_dir = dirname(realpath(__FILE__))."/templates";
		
		$this->name = "foobar";

		$this->release = array("version" => "unknown",
													 "date"    => date("Y-m-d"),
													 "state"   => "",
													 "license" => "unknown",
													 "notes"   => "",
													 );

		$this->constants = array();
		$this->resources = array();
		$this->functions = array();
		$this->internal_functions = array();
		$this->private_functions = array();
		$this->globals   = array();
		$this->phpini    = array();
		$this->users     = array();
		$this->dependson = array();

		$this->files = array("c"=>array(), "h"=>array());
		
		parent::xml_stream_callback_parser($stream);
	}

	// }}} 

		// {{{ parsing

	// {{{   general infos
	  function _check_c_name($name, $msg) {
			if(!ereg("^[[:alpha:]_][[:alnum:]_]*$", $name)) {
				$this->error("$name is not a valid $msg");
			}
    }

		function handle_extension_name($attr) {
			$this->name = trim($this->cdata);
			$this->_check_c_name($this->name, "extension name");
		}

		function handle_extension_summary($attr) {
			$this->summary = trim($this->cdata);
		}

		function handle_extension_description($attr) {
			$this->description = $this->cdata;
		}

		function handle_extension_logo($attr) {
			$this->logo = &new php_logo($this->name, $attr);
		}


		function handle_release_version($attr) {
			$this->release['version'] = trim($this->cdata);
		}

		function handle_release_state($attr) {
			$this->release['state'] = trim($this->cdata);
		}

		function handle_release_license($attr) {
			$this->release['license'] = trim($this->cdata);
		}

	function handle_maintainers_maintainer_user($attr) {
		$this->user["user"] = trim($this->cdata);
	}

	function handle_maintainers_maintainer_name($attr) {
		$this->user["name"] = trim($this->cdata);
	}

	function handle_maintainers_maintainer_email($attr) {
		$this->user["email"] = trim($this->cdata);
	}

	function handle_maintainers_maintainer_role($attr) {
		$this->user["role"] = trim($this->cdata);
	}

	function handle_maintainers_maintainer($attr) {
		$this->users[$this->user["name"]] = $this->user;
		unset($this->user);
	}



		// }}} 

	// {{{   constants

		function handle_constants_constant($attr) {

			$name = $attr["name"];
			$this->_check_c_name($name, "constant name");

			$value= $attr["value"];
			$type = isset($attr["type"]) ? $attr["type"] : "string";

			switch($type) {
			case "int":
			case "integer":
				$this->constants[] = &new php_constant($name, $value, "integer", trim($this->cdata)); 
				break;
				
			case "float":
			case "double":
			case "real":
				if (!is_numeric($value))   $this->error("invalid value for integer constant: '$value'"); 
				$this->constants[] = &new php_constant($name, $value, "float", trim($this->cdata)); 
				break;

			case "string":
			default:
				$this->constants[] = &new php_constant($name, $value, "string", trim($this->cdata)); 
				break;
			}
		}

		// }}} 

	// {{{   resources

	function handle_resources_resource_destruct($attr) {
		$this->resource_destruct = $this->cdata;
	}

	function handle_resources_resource_description($attr) {
		$this->resource_description = $this->cdata;		
	}

	function handle_resources_resource($attr) {
		$this->_check_c_name($attr['name'], "resource name");

		$this->resources[] = new php_resource($attr['name'], 
																					@$attr['payload'], 
																					@$this->resource_destruct, 
																					@$this->resource_description);

		unset($this->resource_description);
		unset($this->resource_destruct);
	}

	// }}} 

	// {{{   functions

		function handle_functions_function_summary($attr) {
			$this->func_summary = trim($this->cdata);
		}

		function handle_functions_function_proto($attr) {
			$this->func_proto = trim($this->cdata);
		}

		function handle_functions_function_description($attr) {
			$this->func_desc = trim($this->cdata);
		}

		function handle_functions_function_code($attr) {
			$this->func_code = $this->cdata;
		}

		function handle_functions_function($attr) {
			$this->_check_c_name($attr['name'], "function name");

			$role = isset($attr['role']) ? $attr['role'] : "public";
			$function = new php_function($attr['name'], @$this->func_summary, @$this->func_proto, @$this->func_desc, @$this->func_code, $role);
			switch($role) {
			case "internal":
				$this->internal_functions[$attr['name']] = $function;
				break;
			case "private":
				$this->private_functions[$attr['name']] = $function;
				break;
			case "public":
				if(is_string($function->status)) $this->error($function->status." in prototype");
				$this->functions[$attr['name']] = $function;
				break;
			default:
				$this->error("function role must be either public, private or internal");
				break;
			}
			unset($this->func_summary);
			unset($this->func_proto);
			unset($this->func_desc);
			unset($this->func_code);
		}

		// }}} 
	
	// {{{   globals and php.ini

		function handle_globals_global($attr) {
			$this->_check_c_name($attr['name'], "variable name");
			if($attr["type"] == "string") $attr["type"] = "char*";
			$this->globals[$attr["name"]] = $attr;
		}

		function handle_globals_phpini($attr) {
			$this->_check_c_name($attr['name'], "php.ini directice name");
			$ini = array("name" => $attr["name"],
									 "type" => $attr["type"],
									 "value"=> $attr["value"],
									 "desc" => trim($this->cdata)
									 );
			switch($attr["access"]) {
			case "system":
				$ini["access"] = "PHP_INI_SYSTEM";
				break;
			case "perdir":
				$ini["access"] = "PHP_INI_PERDIR";
				break;
			case "user":
				$ini["access"] = "PHP_INI_USER";
				break;
			case "all":
			default:
				$ini["access"] = "PHP_INI_ALL";
				break;
			}
			if(isset($attr["onupdate"])) {
				$ini["onupdate"] = $attr["onupdate"];
			} else {
				switch($attr["type"]) {
				case "int":
				case "long":
					$ini["onupdate"] = "OnUpdateInt";
					break;
				case "float":
				case "double":
					$ini["onupdate"] = "OnUpdateFloat";
					break;
				case "string":
					$ini["type"] = "char*";
					// fallthru
				case "char*":
					$ini["onupdate"] = "OnUpdateString";
					break;
				}
			}
			$this->phpini[$attr["name"]] = $ini;
			$this->handle_globals_global($attr);
		}

	// }}} 

	// }}} 

		// {{{ output generation

		// {{{   docbook documentation

		function generate_documentation() {
			system("rm -rf {$this->name}/manual");
			mkdir("{$this->name}/manual");

			$docdir = "{$this->name}/manual/".$this->name;
			mkdir($docdir);

			$fp = fopen("$docdir/reference.xml", "w");
			fputs($fp,
"<?xml version='1.0' encoding='iso-8859-1'?>
<!-- \$Revision$ -->
 <reference id='ref.{$this->name}'>
  <title>{$this->summary}</title>
  <titleabbrev>{$this->name}</titleabbrev>

  <partintro>
   <section id='{$this->name}.intro'>
    &reftitle.intro;
    <para>
{$this->description}
    </para>
   </section>
   
   <section id='{$this->name}.requirements'>
    &reftitle.required;
    <para>
    </para>
   </section>

   &reference.{$this->name}.configure;

   <section id='{$this->name}.configuration'>
    &reftitle.runtime;
");

   	if(empty($this->phpini)) {
			fputs($fp, "    &no.config;\n");
		} else {
			fputs($fp, 
"    <table>
     <title>{$this->name} runtime configuration</title>
			<tgroup cols='3'>
			 <thead>
        <row>
         <entry>directive</entry>
         <entry>default value</entry>
         <entry>descrpition</entry>
        </row>
       </thead>
      <tbody>
");
			foreach($this->phpini as $directive) {
				fputs($fp, 
"    <row>
     <entry>$directive[name]</entry>
     <entry>$directive[value]</entry>
     <entry>$directive[desc]</entry>
    </row>
");
			}
			fputs($fp, 
"     </tbody>
    </tgroup>
   </table>
");
		}

		fputs($fp,
"   </section>

   <section id='{$this->name}.resources'>
    &reftitle.resources;
");

  if (empty($this->resources)) {
    fputs($fp, "   &no.resource;\n");
  } else {
    foreach ($this->resources as $resource) {
      fputs($fp, $resource->docbook_xml($this->name));
    }
  }


  fputs($fp,
"   </section>

   <section id='{$this->name}.constants'>
    &reftitle.constants;
");
  if(empty($this->constants)) {
    fputs($fp, "    &no.constants;\n");
  } else {
    fputs($fp, 
"    <table>
     <title>{$this->name} constants</title>
      <tgroup cols='3'>
       <thead>
        <row>
         <entry>name</entry>
         <entry>value</entry>
         <entry>descrpition</entry>
        </row>
       </thead>
      <tbody>
");
    foreach($this->constants as $constant) {
      fputs($fp, $constant->docbook_xml($this->name));
    }
    fputs($fp, 
"     </tbody>
    </tgroup>
   </table>
");
  }
  fputs($fp,
"   </section>
   
  </partintro>

&reference.{$this->name}.functions;

 </reference>
");
      fputs($fp, php_element::docbook_editor_footer());

			fclose($fp);

			mkdir("$docdir/functions");
			foreach($this->functions as $name => $function) {
				$filename = $docdir . "/functions/" . strtolower(str_replace("_", "-", $name)) . ".xml";
				$funcfile = fopen($filename, "w");
				fputs($funcfile, $function->docbook_xml());
				fclose($funcfile);
			}
		}

		// }}} 

	  // {{{   extension entry

		function generate_extension_entry() {
			return '
/* {{{ '.$this->name.'_module_entry
 */
zend_module_entry '.$this->name.'_module_entry = {
	STANDARD_MODULE_HEADER,
	"'.$this->name.'",
	'.$this->name.'_functions,
	PHP_MINIT('.$this->name.'),     /* Replace with NULL if there is nothing to do at php startup   */ 
	PHP_MSHUTDOWN('.$this->name.'), /* Replace with NULL if there is nothing to do at php shutdown  */
	PHP_RINIT('.$this->name.'),		  /* Replace with NULL if there is nothing to do at request start */
	PHP_RSHUTDOWN('.$this->name.'),	/* Replace with NULL if there is nothing to do at request end   */
	PHP_MINFO('.$this->name.'),
	"'.$this->release['version'].'", 
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_'.strtoupper($this->name).'
ZEND_GET_MODULE('.$this->name.')
#endif
';
		}

	// }}} 

	  // {{{ globals and ini

	  function generate_globals_c() {
			if (empty($this->globals)) return "";
			
			$code = "ZEND_DECLARE_MODULE_GLOBALS({$this->name})\n\n";
			
			if (!empty($this->phpini)) {
				$code .= "PHP_INI_BEGIN()\n";
				foreach ($this->phpini as $name => $ini) {
					$code .= "  STD_PHP_INI_ENTRY(\"{$this->name}.$name\", \"$ini[value]\", $ini[access], $ini[onupdate], $name, zend_{$this->name}_globals, {$this->name}_globals)\n";
				}
				$code .= "PHP_INI_END()\n\n";			
				$code .= "static void php_{$this->name}_init_globals(zend_{$this->name}_globals *{$this->name}_globals)\n";
				$code .= "{\n";
				foreach ($this->globals as $name => $ini) {
					$code .= "  {$this->name}_globals->$name = ";
					if (strstr($ini["type"],"*")) {
						$code .= "NULL;\n";
					} else {
						$code .= "0;\n";
					}
				}
				$code .= "}\n\n";
				return $code;
			}
		}
		
	  function generate_globals_h() {
			if (empty($this->globals)) return "";
			
			$code = "ZEND_BEGIN_MODULE_GLOBALS({$this->name})\n";
			foreach($this->globals as $name => $global) {
				$code .= "  $global[type] $name;\n";
			}
			$code.= "ZEND_END_MODULE_GLOBALS({$this->name})\n";
			
			$upname = strtoupper($this->name);
			
			$code.= "

#ifdef ZTS
#define {$upname}_G(v) TSRMG({$this->name}_globals_id, zend_{$this->name}_globals *, v)
#else
#define {$upname}_G(v) ({$this->name}_globals.v)
#endif

";
			
			return $code;
		}

	// }}} 

	// {{{ license and authoers

	  function get_license() {
			$code = "/*\n";
			switch($this->release['license']) {
			case "php":
				$code.=
'   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
';
				break;
			
			default:
				$code.= 
"   +----------------------------------------------------------------------+
   | unkown license: '{$this->release['license']}'                                                        |
   +----------------------------------------------------------------------+
";
				break;
			}

			$code.= "   +----------------------------------------------------------------------+\n";
			$prefix = "Authors: ";
			foreach($this->users as $name => $user) {
				$code .= sprintf("   | $prefix %-58s |\n", "$user[name] <$user[email]>");
				$prefix = str_repeat(" ",strlen($prefix));
			}
			$code.= "   +----------------------------------------------------------------------+\n";
			$code.= "*/\n\n";

			$code.= "/* $ Id: $ */ \n\n";

			return $code;
	  }

	// }}} 

	// {{{ editor config footer

	  function editor_config_c() {
			return '
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
';
	  }

	// }}} 

	// {{{ header file

	  function write_header_file() {
			$filename = "php_{$this->name}.h";

			$this->files["h"][] = $filename; 
			$fp = fopen("{$this->name}/$filename", "w");

			$upname = strtoupper($this->name);
			
			fputs($fp, $this->get_license());
			fputs($fp, "#ifndef PHP_{$upname}_H\n");
			fputs($fp, "#define PHP_{$upname}_H\n\n");

			fputs($fp, "#ifndef PHP_HAVE_{$upname}\n\n");

			fputs($fp, "
extern zend_module_entry {$this->name}_module_entry;
#define phpext_{$this->name}_ptr &{$this->name}_module_entry

#ifdef PHP_WIN32
#define PHP_{$upname}_API __declspec(dllexport)
#else
#define PHP_{$upname}_API
#endif

PHP_MINIT_FUNCTION({$this->name});
PHP_MSHUTDOWN_FUNCTION({$this->name});
PHP_RINIT_FUNCTION({$this->name});
PHP_RSHUTDOWN_FUNCTION({$this->name});
PHP_MINFO_FUNCTION({$this->name});

#ifdef ZTS
#include \"TSRM.h\"
#endif

");

			fputs($fp, $this->generate_globals_h());

			fputs($fp, "\n");

			foreach($this->functions as $name => $function) {
				fputs($fp, "PHP_FUNCTION($name);\n");
			}
			
			fputs($fp, "\n");

			fputs($fp, "#endif /* PHP_HAVE_{$upname} */\n\n");
			fputs($fp, "#endif /* PHP_{$upname}_H */\n\n");

			fputs($fp, $this->editor_config_c());

			fclose($fp);
		}

	// }}} 

	// {{{ internal functions

	function internal_functions_c() {
		$need_block = false;

		$code = "
/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION({$this->name})
{
";

		if (count($this->globals)) {
			$code .= "\tZEND_INIT_MODULE_GLOBALS({$this->name}, php_{$this->name}_init_globals, NULL)\n";
			$need_block = true;
		}

		if (count($this->phpini)) {
			$code .= "\tREGISTER_INI_ENTRIES();\n";
			$need_block = true;
		}

		if (isset($this->logo)) {
			$code .= $this->logo->minit_code();
			$need_block = true;
		}

		if (count($this->constants)) {
			foreach ($this->constants as $constant) {
				$code .= $constant->c_code();
			}
			$need_block = true;
		}

		if (count($this->resources)) {
			foreach ($this->resources as $resource) {
				$code .= $resource->minit_code();
      }
			$need_block = true;			
    }

		if (isset($this->internal_functions['MINIT'])) {
      if($need_block) $code .= "\n\t{\n";
			$code .= $this->internal_functions['MINIT']->code;
      if($need_block) $code .= "\n\t}\n";
		} else {
			$code .="\n\t/* add your stuff here */\n";
		}
		$code .= "
  return SUCCESS;
}
/* }}} */

";

		$code .= "
/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION({$this->name})
{
";

		if(count($this->phpini)) {
			$code .= "\tUNREGISTER_INI_ENTRIES();\n";
		}

		if(isset($this->internal_functions['MSHUTDOWN'])) {
      if(count($this->phpini)) $code .= "\n\t{\n";
			$code .= $this->internal_functions['MSHUTDOWN']->code;
      if(count($this->phpini)) $code .= "\n\t}\n";
		} else {
		  $code .="\n\t/* add your stuff here */\n";
    }

		$code .= "
  return SUCCESS;
}
/* }}} */

";

		$code .= "
/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION({$this->name})
{
";

		if(isset($this->internal_functions['RINIT'])) {
			$code .= $this->internal_functions['RINIT']->code;
		} else {
       $code .= "  /* add your stuff here */\n";
    }

    $code .= "
\treturn SUCCESS;
}
/* }}} */

";

		$code .= "
/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION({$this->name})
{
";

		if(isset($this->internal_functions['RSHUTDOWN'])) {
			$code .= $this->internal_functions['RSHUTDOWN']->code;
		} else {
       $code .= "  /* add your stuff here */\n";
    }

    $code .= "
\treturn SUCCESS;
}
/* }}} */

";
	
  $code .= "
/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION({$this->name})
{
	php_info_print_box_start(0);
";

   if(isset($this->logo))
   {
     $code.= "
	php_printf(\"<img src='\");
	if (SG(request_info).request_uri) {
		php_printf(\"%s\", (SG(request_info).request_uri));
	}	
	php_printf(\"?=%s\", ".($this->logo->id).");
	php_printf(\"' align={'right' alt='image' border='0'>\\n\");

"; 
    }

   if(isset($this->summary)) {
     $code .= "  php_printf(\"<p>{$this->summary}</p>\\n\");\n";
   }
   if(isset($this->release)) {
     $code .= "  php_printf(\"<p>Version {$this->release['version']}{$this->release['state']} ({$this->release['date']})</p>\\n\");\n";
   }

   if(count($this->users)) {
     $code .= "  php_printf(\"<p><b>Authors:</b></p>\\n\");\n";
     foreach($this->users as $user) {
       $code .= "  php_printf(\"<p>$user[name] &lt;$user[email]&gt; ($user[role])</p>\\n\");\n";
     }
   }

    $code.=
"	php_info_print_box_end();
";

		if(isset($this->internal_functions['MINFO'])) {
      $code .= "\n\t{\n";
			$code .= $this->internal_functions['MINFO']->code;
      $code .= "\n\t}\n";
		} else {
       $code .= "\t/* add your stuff here */\n";
    }


if(count($this->phpini)) {
	$code .= "\n\tDISPLAY_INI_ENTRIES();";
}
$code .= "
}
/* }}} */

";

		return $code;
  }

	// }}} 


	function private_functions_c() {
		$code = "";

		foreach ($this->private_functions as $name => $func) {
      $code .= "\n\t/* {{{ $name() */\n{$func->code}\n\t/* }}} */\n\n";
    }

  	return $code;
  }

	// {{{ public functions

	function public_functions_c() {
		$code = "";

    foreach($this->functions as $function) {
      $code .= $function->c_code();
    }
		
		return $code;
	}

	// }}} 


  // {{{ code file

	function write_code_file() {
			$filename = "{$this->name}.c";

			$this->files["c"][] = $filename; 
			$fp = fopen("{$this->name}/$filename", "w");

			$upname = strtoupper($this->name);
			
			fputs($fp, $this->get_license());

			fputs($fp, '
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>

');
			fputs($fp, "#include \"php_{$this->name}.h\"\n\n");
						
      if (isset($this->logo)) {
			  fputs($fp, $this->logo->c_code());	
      }

			if (!empty($this->globals)) {
				fputs($fp, "ZEND_DECLARE_MODULE_GLOBALS({$this->name})\n\n");
			}

			if (!empty($this->resources)) {
				foreach ($this->resources as $resource) {
					fputs($fp, $resource->c_code());
				}
			}

			fputs($fp, "/* {{{ {$this->name}_functions[] */\n");
			fputs($fp, "function_entry {$this->name}_functions[] = {\n");
			foreach($this->functions as $name => $function) {
				fputs($fp, sprintf("\tPHP_FE(%-20s, NULL)\n",$name));
			}
			fputs($fp, "};\n/* }}} */\n\n");
			
			fputs($fp, $this->generate_extension_entry());

			fputs($fp, "\n/* {{{ globals and ini entries */\n");
			fputs($fp, $this->generate_globals_c());
			fputs($fp, "/* }}} */\n\n");

			fputs($fp, $this->internal_functions_c());

			fputs($fp, $this->private_functions_c());

			fputs($fp, $this->public_functions_c());

			fputs($fp, $this->editor_config_c());
	}

	// }}} 


	// {{{ config.m4 file
	function write_config_m4() {

		$upname = $this->name;
		
		$fp = fopen("{$this->name}/config.m4", "w");
		fputs($fp, 
"dnl
dnl \$ Id: \$
dnl

PHP_ARG_ENABLE({$this->name} , whether to enable {$this->name} functions,
[  --disable-{$this->name}         Disable {$this->name} functions], yes)

if test \"\$PHP_$upname\" != \"no\"; then
  AC_DEFINE(HAVE_$upname, 1, [ ])
  PHP_NEW_EXTENSION({$this->name}, ".join(" ", $this->files['c'])." , \$ext_shared)
fi
");
		fclose($fp);
	}

	// }}} 

	// {{{ M$ dev studio project file

	function write_ms_devstudio_dsp() {
    // TODO files should come from external list

		$fp = fopen("{$this->name}/{$this->name}.dsp","w");
		fwrite($fp,
'# Microsoft Developer Studio Project File - Name="'.$this->name.'" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG='.$this->name.' - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "'.$this->name.'.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "'.$this->name.'.mak" CFG="'.$this->name.' - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "'.$this->name.' - Win32 Release_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "'.$this->name.' - Win32 Debug_TS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "'.$this->name.' - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_TS"
# PROP BASE Intermediate_Dir "Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "'.strtoupper($this->name).'_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\main" /D "WIN32" /D "PHP_EXPORTS" /D "COMPILE_DL_'.strtoupper($this->name).'" /D ZTS=1 /D HAVE_'.strtoupper($this->name).'=1 /D ZEND_DEBUG=0 /D "NDEBUG" /D "_WINDOWS" /D "ZEND_WIN32" /D "PHP_WIN32" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php4ts.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_'.$this->name.'.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "'.$this->name.' - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_TS"
# PROP BASE Intermediate_Dir "Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_TS"
# PROP Intermediate_Dir "Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "'.strtoupper($this->name).'_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\Zend" /I "..\..\TSRM" /I "..\..\main" /D ZEND_DEBUG=1 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "PHP_EXPORTS" /D "COMPILE_DL_'.strtoupper($this->name).'" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D HAVE_'.strtoupper($this->name).'=1 /YX /FD /GZ  /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php4ts_debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_'.$this->name.'.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"

!ENDIF 

# Begin Target

# Name "'.$this->name.' - Win32 Release_TS"
# Name "'.$this->name.' - Win32 Debug_TS"
');


		fputs($fp,'
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
');

		foreach($this->files['c'] as $filename) {
			if($filename{0}!='/' && $filename{0}!='.') {
				$filename = "./$filename";
			}
			$filename = str_replace("/","\\",$filename);

			fputs($fp,"
# Begin Source File

SOURCE=$filename
# End Source File
");
		}

fputs($fp,'
# End Group
');




fputs($fp,'
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
');
		foreach($this->files['h'] as $filename) {
			if($filename{0}!='/' && $filename{0}!='.') {
				$filename = "./$filename";
			}
			$filename = str_replace("/","\\",$filename);

			fputs($fp,"
# Begin Source File

SOURCE=$filename
# End Source File
");
		}

fputs($fp,
'# End Group
# End Target
# End Project
');



fclose($fp);
	}

// }}} 

	  // }}} 

	}	

?>