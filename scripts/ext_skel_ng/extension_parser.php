<?php
	// {{{ includes

	require_once "php_element.php";
	require_once "php_constant.php";
	require_once "php_function.php";

	require_once "xml_stream_parser.php";
	require_once "xml_stream_callback_parser.php";

	// }}} 

	class extension_parser extends xml_stream_callback_parser {

		// {{{ constructor

	function extension_parser($stream) {
		$this->template_dir = dirname(realpath(__FILE__))."/templates";
		
		$this->name = "foobar";

		$this->license = "php";
		
		$this->constants = array();
		$this->functions = array();
		$this->globals   = array();
		$this->phpini    = array();
		$this->users     = array();
		$this->dependson = array();
		
		parent::xml_stream_callback_parser($stream);
	}

	// }}} 

		// {{{ parsing

	// {{{   general infos

		function handle_extension_name($attr) {
			$this->name = trim($this->cdata);
		}

		function handle_extension_summary($attr) {
			$this->summary = trim($this->cdata);
		}

		function handle_extension_description($attr) {
			$this->description = $this->cdata;
		}

		function handle_release_version($attr) {
			$this->version = trim($this->cdata);
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
			$value= $attr["value"];
			$type = isset($attr["type"]) ? $attr["type"] : "string";

			switch($type) {
			case "int":
			case "integer":
				if (!is_numeric($value))   $this->error("invalid value for integer constant: '$value'"); 
				if ((int)$value != $value) $this->error("invalid value for integer constant: '$value'");
				$this->constants[] = &new php_constant($name, (int)$value, "integer", trim($this->cdata)); 
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
			$this->functions[$attr['name']] = new php_function($attr['name'], $this->func_summary, $this->func_proto, @$this->func_desc, @$this->func_code);
			unset($this->func_summary);
			unset($this->func_proto);
			unset($this->func_desc);
			unset($this->func_code);
		}

		// }}} 
	
	// {{{   globals and php.ini

		function handle_globals_global($attr) {
			if($attr["type"] == "string") $attr["type"] = "char*";
			$this->globals[$attr["name"]] = $attr;
		}

		function handle_globals_phpini($attr) {
			$ini = array("name" => $attr["name"],
									 "type" => $attr["type"],
									 "value"=> $attr["value"]
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
    &no.config;
   </section>

   <section id='{$this->name}.resources'>
    &reftitle.resources;
    &no.resource;
   </section>

   <section id='{$this->name}.constants'>
    &reftitle.constants;
    &no.constants;
   </section>
   
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
	"'.$this->version.'", 
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
			switch($this->license) {
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
   | unkown license: '{$this->license}'                                                        |
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
			$fp = fopen("{$this->name}/php_{$this->name}.h", "w");

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
		$code = "
/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION({$this->name})
{
";

		if(count($this->globals)) {
			$code .= "  ZEND_INIT_MODULE_GLOBALS({$this->name}, php_{$this->name}_init_globals, NULL)\n";
		}

		if(count($this->phpini)) {
			$code .= "  REGISTER_INI_ENTRIES();\n";
		}

		$code .="\n  /* add your stuff here */\n";

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
			$code .= "  UNREGISTER_INI_ENTRIES();\n";
		}

		$code .="\n  /* add your stuff here */\n";

		$code .= "
  return SUCCESS;
}
/* }}} */

";

		$code .= "
/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION({$this->name})
{
  /* add your stuff here */

   return SUCCESS;
}
/* }}} */

";

		$code .= "
/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION({$this->name})
{
  /* add your stuff here */

  return SUCCESS;
}
/* }}} */

";
	
  $code .= "
/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION({$this->name})
{
  php_info_print_table_start();
  php_info_print_table_header(2, \"{$this->name} support\", \"enabled\");
  php_info_print_table_end();

  /* add your stuff here */
";

if(count($this->phpini)) {
	$code .= "\n  DISPLAY_INI_ENTRIES();";
}
$code .= "
}
/* }}} */

";

		return $code;
  }

	// }}} 


	// {{{ public functions

	function public_functions_c() {
		$code = "";

		foreach ($this->functions as $name => $func) {

			$code .= "\n/* {{{ func {$func->returns} {$func->name}(";
			if (isset($func->params)) {
				foreach ($func->params as $key => $param) {
					if (!empty($param['optional'])) $code.=" [";
					if ($key) $code.=", ";
					$code .= $param['type']." ";
					$code .= isset($param['name']) ? $param['name'] : "par_$key"; 
				}
			}
			for ($n=$func->optional; $n>0; $n--) {
				$code .= "]";
			}
			$code .= ")\n  ";
			if(!empty($func->summary)) $code .= $func->summary;
			$code .= " */\n";

			$code .= "PHP_FUNCTION({$func->name})\n";
			$code .= "{\n";
			$code .= "\tint argc = ZEND_NUM_ARGS();\n\n";
			
			if (isset($func->params)) {
				$arg_string="";
				$arg_pointers=array();
				$optional=false;
				$res_fetch="";
				
				foreach ($func->params as $key => $param) {
					$name = isset($param['name']) ? $param['name'] : "par_$key"; 
					$arg_pointers[]="&$name";
					if(isset($param['optional'])&&!$optional) {
						$optional=true;
						$arg_string.="|";
					}

					switch($param['type']) {
					case "void":
						break;

					case "bool":
						$arg_string.="b";
						$code .= "\tzend_bool $name = 0;\n";
						break;

					case "int":
						$arg_string.="l";
						$code .= "\tlong $name = 0;\n";
						break;

					case "float":
						$arg_string.="d";
						$code .= "\tdouble $name = 0.0;\n";
						break;

					case "string":
						$arg_string.="s";
						$code .= "\tchar * $name = NULL;\n";
						$code .= "\tint {$name}_len = 0;\n";
						$arg_pointers[]="&{$name}_len";
						break;

					case "array":
						$arg_string.="a";
						$code .= "\tzval * $name = NULL;\n";
						break;

					case "object": 
						$arg_string.="o";
						$code .= "\tzval * $name = NULL;\n";
						break;

					case "resource":
						$arg_string.="r";
						$code .= "\tzval * $name = NULL;\n";
						$code .= "\tint * {$name}_id = -1;\n";
						$arg_pointers[]="&{$name}_id";
						$res_fetch.="\tif ($name) {\n"
							."\t\tZEND_FETCH_RESOURCE(???, ???, $name, {$name}_id, \"???\", ???_rsrc_id);\n"
							."\t}\n";
						break;

					case "mixed":
						$arg_string.="z";
						$code .= "\tzval * $name = NULL;\n";
						break;
					}
				}

				$code .= "\n\tif (zend_parse_parameters(argc TSRMLS_CC, \"$arg_string\", ".join(", ",$arg_pointers).") == FAILURE) return;\n";
				if($res_fetch) $code.="\n$res_fetch\n";
			} else {
				$code .= "\tif(argc>0) { WRONG_PARAM_COUNT; }\n";
			}

      $code .= "\n";

      if (!empty($func->code)) {
        $code .= $func->code."\n";
      } else {
			  $code .= "\tphp_error(E_WARNING, \"{$func->name}: not yet implemented\");\n\n";
			
			  switch($func->returns) {
			  case "void":
				  break;
				
			  case "bool":
				  $code .= "\tRETURN_FALSE;\n"; 
				  break;
				
			  case "int":
			  	$code .= "\tRETURN_LONG(0);\n"; 
				  break;
				
			  case "float":
				  $code .= "\tRETURN_DOUBLE(0.0);\n";
				  break;
				
			  case "string":
				  $code .= "\tRETURN_STRINGL(\"\", 0, 1);\n";
				  break;

			  case "array":
			  	$code .= "\tarray_init(return_value);\n";
				  break;
				
			  case "object": 
				  $code .= "\tobject_init(return_value)\n";
				  break;
				
			  case "resource":
				  $code .= "\t/* RETURN_RESOURCE(...); /*\n";
				  break;

			  case "mixed":
				  $code .= "\t/* RETURN_...(...); /*\n";				
				  break;
			  }
      }
			

			$code .= "}\n/* }}} */\n\n";
		}

		
		return $code;
	}

	// }}} 


  // {{{ code file

	function write_code_file() {
			$fp = fopen("{$this->name}/{$this->name}.c", "w");

			$upname = strtoupper($this->name);
			
			fputs($fp, $this->get_license());

			fputs($fp, '
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

');
			fputs($fp, "#include \"php_{$this->name}.h\"\n\n");
						
			if (!empty($this->globals)) {
				fputs($fp, "ZEND_DECLARE_MODULE_GLOBALS({$this->name})\n\n");
			}

			fputs($fp, "/* {{{ {$this->name}_functions[] */\n");
			fputs($fp, "function_entry {$this->name}_functions[] = {\n");
			foreach($this->functions as $name => $function) {
				fputs($fp, sprintf("  PHP_FE(%-20s, NULL)\n",$name));
			}
			fputs($fp, "};\n/* }}} */\n\n");
			
			fputs($fp, $this->generate_extension_entry());

			fputs($fp, "\n/* {{{ globals and ini entries */\n");
			fputs($fp, $this->generate_globals_c());
			fputs($fp, "/* }}} */\n\n");

			fputs($fp, $this->internal_functions_c());

			fputs($fp, $this->public_functions_c());

			fputs($fp, $this->editor_config_c());
	}

	// }}} 


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
  PHP_NEW_EXTENSION({$this->name}, {$this->name}.c, \$ext_shared)
fi
");
		fclose($fp);
	}

	  // }}} 

	}	

?>