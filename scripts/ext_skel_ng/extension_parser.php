<?php
	// {{{ includes

	require_once "php_element.php";
	require_once "php_constant.php";
	require_once "php_function.php";
	require_once "php_resource.php";
	require_once "php_ini.php";
	require_once "php_global.php";
	require_once "php_logo.php";

	require_once "license.php";

	require_once "xml_stream_parser.php";
	require_once "xml_stream_callback_parser.php";

	// }}} 

	class extension_parser extends xml_stream_callback_parser {

		// {{{ constructor

	function __construct($stream) {
		$this->name = "extension";

		$this->release = array("version" => "unknown",
													 "date"    => date("Y-m-d"),
													 "state"   => "",
													 "license" => "unknown",
													 "notes"   => "",
													 );

		$this->constants = array();
		$this->resources = array();
		$this->streams   = array();
		$this->functions = array();
		$this->internal_functions = array();
		$this->private_functions = array();
		$this->globals   = array();
		$this->phpini    = array();
		$this->users     = array();
		$this->dependson = array();
		$this->code      = array();
		$this->libs      = array();
		$this->headers   = array();
		$this->language  = "c";
		$this->platform  = "all";

		$this->files = array("c"    => array(), 
												 "h"    => array(),
												 "conf" => array(),
												 "doc"  => array()
												 );
		
		parent::__construct($stream);
	}

	// }}} 

	// {{{ helper methods

	  function _check_c_name($name, $msg) {
			if (!ereg("^[[:alpha:]_][[:alnum:]_]*$", $name)) {
				$this->error("$name is not a valid $msg");
			}
    }

    function _trimdata() {
      $text = preg_replace('|^\s*\n|m','', $this->cdata);
      $text = preg_replace('|\n\s*$|m',"\n", $text);
      return $text;
    }


	// }}}

		// {{{ parsing

	// {{{   general infos
		function handle_extension_name($attr) {
			$this->name = trim($this->cdata);
			$this->_check_c_name($this->name, "extension name");
		}

		function handle_extension_summary($attr) {
			$this->summary = trim($this->cdata);
		}

		function handle_extension_description($attr) {
			$this->description = $this->_trimdata();
		}

		function handle_extension_logo($attr) {
			$this->logo = new php_logo($this->name, $attr);
		}


		function handle_release_version($attr) {
			$this->release['version'] = trim($this->cdata);
		}

		function handle_release_state($attr) {
			$this->release['state'] = trim($this->cdata);
		}

		function handle_release_license($attr) {
			$this->release['license'] = trim($this->cdata);
			$this->license = license::factory($this->release['license']);
			if (is_object($this->license)) {
				$this->license->write_license_file();
				$this->files['doc'][] = "LICENSE";
			}
		}

		function handle_release_notes($attr) {
			$this->release['notes'] = trim($this->cdata);
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

  // {{{   dependencies

  function handle_deps($attr) {
    if (isset($attr["language"])) $this->language = $attr["language"];
		if (isset($attr['platform'])) {
			if ($attr['platform'] != "all") {
				$this->platform = split("[[:space:]]*,[[:space:]]*", trim($attr['platform']));
			}
		} else {
			$this->platform = "all";
		}
  }

	function handle_deps_lib($attr) {
		$this->libs[$attr['name']] = $attr;
		if (isset($attr['platform'])) {
			if ($attr['platform'] != "all") {
				$this->libs[$attr['name']]['platform'] = split("[[:space:]]*,[[:space:]]*", trim($attr['platform']));
			}
		} else {
			$this->libs[$attr['name']]['platform'] = "all";
		}
	}

	function handle_deps_header($attr) {
		$this->headers[$attr['name']] = $attr; 
	}

	function handle_deps_with($attr) {
		$this->with = array();
		$this->with['attr'] = $attr;
		$this->with['desc'] = trim($this->cdata);		
	}

  // }}}

	// {{{   constants

	function handle_constants_constant($attr) {
		$constant = new php_constant($attr, $this->_trimdata);
		if (isset($constant->error)) {
			$this->error($constant->error);
		}
		$this->constants[$attr['name']] = $constant;
	}

		// }}} 

  // {{{   streams
  function hamdle_steams($attr) {
	}

  function hamdle_steams_stream($attr) {
		$this->streams[$attr['name']] = new php_stream($attr, $this->stream_data);
		unset($this->stream_data);
	}

  function hamdle_steams_stream_description($attr) {
		$this->stream_data['desc'] = $this->_trimdata;
	}

  function hamdle_steams_stream_ops($attr) {
	}

  function hamdle_steams_stream_ops_op($attr) {
	}

  function hamdle_steams_stream_data($attr) {
	}
	// }}}

	// {{{   resources

	function handle_resources_resource_destruct($attr) {
		$this->resource_destruct = $this->_trimdata();
	}

	function handle_resources_resource_description($attr) {
		$this->resource_description = $this->_trimdata();		
	}

	function handle_resources_resource($attr) {
    $resource = new php_resource($attr,
																 @$this->resource_destruct, 
																 @$this->resource_description
																 );
		
		if (isset($resource->error)) {
			$this->error($resource->error);
		}

		$this->resources[$attr['name']] = $resource;
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
			$this->func_desc = $this->_trimdata();
		}

		function handle_functions_function_code($attr) {
			$this->func_code = $this->_trimdata();
		}

		function handle_code($attr) {
			$this->code[$attr["role"]][] = $this->_trimdata();
		}

		function handle_functions_function($attr) {
			$this->_check_c_name($attr['name'], "function name");

			$role = isset($attr['role']) ? $attr['role'] : "public";
			$function = new php_function($attr['name'], 
																	 @$this->func_summary, 
																	 @$this->func_proto, 
																	 @$this->func_desc, 
																	 @$this->func_code, 
																	 $role);

			switch ($role) {
			case "internal":
				$this->internal_functions[$attr['name']] = $function;
				break;
			case "private":
				$this->private_functions[$attr['name']] = $function;
				break;
			case "public":
				if (is_string($function->status)) $this->error($function->status." in prototype");
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
			$this->globals[$attr["name"]] = new php_global($attr);
			if (isset($this->globals[$attr["name"]]->error)) {
				$this->error($this->globals[$attr["name"]]->error);
			}
		}

		function handle_globals_phpini($attr) {
			$attr["desc"] = $this->_trimdata();

			$ini = new php_ini($attr);
			if (isset($ini->error)) {
				$this->error($ini->error);
			}
			$this->phpini[$attr['name']] = $ini;
			
			// php.ini settings are stored in modul-global variables
			$attr['type'] = $ini->c_type;
			$global = new php_global($attr);
			$this->globals[$attr["name"]] = $global;
			if (isset($global->error)) {
				$this->error($global->error);
			}
		}

	// }}} 

	// }}} 

		// {{{ output generation

		// {{{   docbook documentation

		function generate_documentation() {
			$id_name = str_replace('_', '-', $this->name);

			mkdir("{$this->name}/manual");

			$docdir = "{$this->name}/manual/$id_name";
			mkdir($docdir);

			$fp = fopen("$docdir/reference.xml", "w");
			fputs($fp,
"<?xml version='1.0' encoding='iso-8859-1'?>
<!-- ".'$'."Revision: 1.1 $ -->
 <reference id='ref.$id_name'>
  <title>{$this->summary}</title>
  <titleabbrev>$id_name</titleabbrev>

  <partintro>
   <section id='$id_name.intro'>
    &reftitle.intro;
    <para>
{$this->description}
    </para>
   </section>
   
   <section id='$id_name.requirements'>
    &reftitle.required;
    <para>
    </para>
   </section>

   &reference.$id_name.configure;

   <section id='$id_name.resources'>
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

   <section id='$id_name.constants'>
    &reftitle.constants;
");

  if (empty($this->constants)) {
    fputs($fp, "    &no.constants;\n");
  } else {
    fputs($fp, php_constant::docbook_xml_header($id_name));

    foreach ($this->constants as $constant) {
      fputs($fp, $constant->docbook_xml($this->name));
    }

    fputs($fp, php_constant::docbook_xml_footer());
  }
  fputs($fp,
"   </section>
   
  </partintro>

&reference.$id_name.functions;

 </reference>
");
      fputs($fp, php_element::docbook_editor_settings());

			fclose($fp);

			// configure options and dependencies have their own file
			$fp = fopen("$docdir/configure.xml","w");

      fputs($fp,"\n   <section id='$id_name.requirements'>\n    &reftitle.required;\n");
     	if (empty($this->libs) && empty($this->headers)) {
			  fputs($fp, "    &no.requirement;\n");
		  } else {
        // TODO allow custom text
        if (isset($this->libs)) {
          $libs = array();
          foreach ($this->libs as $lib) {
            $libs[] = $lib['name'];
          }
          $ies = count($libs)>1 ? "ies" :"y";
          fputs($fp, "<para>This extension requires the following librar$ies: ".join(",", $libs)."</para>\n");
        }
        if (isset($this->headers)) {
          $headers = array();
          foreach ($this->headers as $header) {
            $headers[] = $header['name'];
          }
          $s = count($headers)>1 ? "s" : "";
          fputs($fp, "<para>This extension requires the following header$s: ".join(",", $headers)."</para>\n");
        }
		  }
      fputs($fp, "\n   </section>\n\n");

      fputs($fp,"\n   <section id='$id_name.install'>\n    &reftitle.install;\n");
     	if (empty($this->with)) {
			  fputs($fp, "    &no.install;\n");
		  } else {
			  if (isset($this->with['desc'])) {
				  if (strstr($this->with['desc'], "<para>")) {
					  fputs($fp, $this->with['desc']);
					} else {
					  fputs($fp, "    <para>\n".rtrim($this->with['desc'])."\n    </para>\n");
          }
        } else {
				  // TODO default text
        }
		  }
      fputs($fp, "\n   </section>\n\n");

      fputs($fp,"\n   <section id='$id_name.configuration'>\n    &reftitle.runtime;\n");
     	if (empty($this->phpini)) {
			  fputs($fp, "    &no.config;\n");
		  } else {
			  fputs($fp, php_ini::docbook_xml_header($this->name)); 
			  foreach ($this->phpini as $phpini) {
				  fputs($fp, $phpini->docbook_xml());
			  }
			  fputs($fp, php_ini::docbook_xml_footer()); 
		  }
      fputs($fp, "\n   </section>\n\n");
			
      fputs($fp, php_element::docbook_editor_settings());
			fclose($fp);

			mkdir("$docdir/functions");
			foreach ($this->functions as $name => $function) {
				$filename = $docdir . "/functions/" . strtolower(str_replace("_", "-", $name)) . ".xml";
				$funcfile = fopen($filename, "w");
				fputs($funcfile, $function->docbook_xml());
				fclose($funcfile);
			}
		}

		// }}} 

	  // {{{   extension entry

		function generate_extension_entry() {
      $name = $this->name;
      $upname = strtoupper($this->name);

			$code = "
/* {{{ {$name}_module_entry
 */
zend_module_entry {$name}_module_entry = {
	STANDARD_MODULE_HEADER,
	\"$name\",
	{$name}_functions,
	PHP_MINIT($name),     /* Replace with NULL if there is nothing to do at php startup   */ 
	PHP_MSHUTDOWN($name), /* Replace with NULL if there is nothing to do at php shutdown  */
	PHP_RINIT($name),		  /* Replace with NULL if there is nothing to do at request start */
	PHP_RSHUTDOWN($name),	/* Replace with NULL if there is nothing to do at request end   */
	PHP_MINFO($name),
	\"".$this->release['version']."\", 
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

";

      $code .= "#ifdef COMPILE_DL_$upname\n";
	    if ($this->language == "cpp") $code .= "extern \"C\" {\n";
      $code .= "ZEND_GET_MODULE($name)\n";
	    if ($this->language == "cpp") $code .= "}\n";
	    $code .= "#endif\n\n";

			return $code;
		}

	// }}} 

	  // {{{ globals and ini

	  function generate_globals_c() {
			if (empty($this->globals)) return "";
			
			$code = "ZEND_DECLARE_MODULE_GLOBALS({$this->name})\n\n";
			
			if (!empty($this->phpini)) {
				$code .= php_ini::c_code_header($this->name);
				foreach ($this->phpini as $phpini) {
          $code .= $phpini->c_code($this->name);
				}
				$code .= php_ini::c_code_footer();
			}

			if (!empty($this->globals)) {
				$code .= php_global::c_code_header($this->name);
				foreach ($this->globals as $global) {
          $code .= $global->c_code($this->name);
				}
				$code .= php_global::c_code_footer();
			}

			return $code;
		}
		
	  function generate_globals_h() {
			if (empty($this->globals)) return "";
			
			$code = php_global::h_code_header($this->name);
			foreach ($this->globals as $global) {
        $code .= $global->h_code($this->name);
			}
			$code .= php_global::h_code_footer($this->name);
			
			return $code;
		}

	// }}} 

	// {{{ license and authoers

	  function get_license() {

			$code = "/*\n";
			$code.= "   +----------------------------------------------------------------------+\n";

      if (is_object($this->license)) {
        $code.= $this->license->license_comment();
      } else {
        $code.= sprintf("   | unkown license: %-52s |\n", $this->release['license']);
			}

			$code.= "   +----------------------------------------------------------------------+\n";
			$prefix = "Authors: ";
			foreach ($this->users as $name => $user) {
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

			if (isset($this->headers)) {
				foreach ($this->headers as $header) {
					if (@$header["prepend"] === "yes") {
						fputs($fp, "#include <$header[name]>\n");
					}
				}
			}

			if ($this->language == "cpp") {
				fputs($fp, "extern \"C\" {\n");
			}

			fputs($fp, '
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_'.$upname.'

#include <php.h>
#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>

');

			if ($this->language == "cpp") {
				fputs($fp, "}\n");
			}

			if (isset($this->headers)) {
				foreach ($this->headers as $header) {
					if (@$header["prepend"] !== "yes") {
						fputs($fp, "#include <$header[name]>\n");
					}
				}
			}

			if (isset($this->code["header"])) {
				foreach ($this->code["header"] as $code) {
					fputs($fp, $code);
				}
			}

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

			foreach ($this->functions as $name => $function) {
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
      if ($need_block) $code .= "\n\t{\n";
			$code .= $this->internal_functions['MINIT']->code;
      if ($need_block) $code .= "\n\t}\n";
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

		if (count($this->phpini)) {
			$code .= "\tUNREGISTER_INI_ENTRIES();\n";
		}

		if (isset($this->internal_functions['MSHUTDOWN'])) {
      if (count($this->phpini)) $code .= "\n\t{\n";
			$code .= $this->internal_functions['MSHUTDOWN']->code;
      if (count($this->phpini)) $code .= "\n\t}\n";
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

		if (isset($this->internal_functions['RINIT'])) {
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

		if (isset($this->internal_functions['RSHUTDOWN'])) {
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

   if (isset($this->logo))
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

   if (isset($this->summary)) {
     $code .= "  php_printf(\"<p>{$this->summary}</p>\\n\");\n";
   }
   if (isset($this->release)) {
     $code .= "  php_printf(\"<p>Version {$this->release['version']}{$this->release['state']} ({$this->release['date']})</p>\\n\");\n";
   }

   if (count($this->users)) {
     $code .= "  php_printf(\"<p><b>Authors:</b></p>\\n\");\n";
     foreach ($this->users as $user) {
       $code .= "  php_printf(\"<p>$user[name] &lt;$user[email]&gt; ($user[role])</p>\\n\");\n";
     }
   }

    $code.=
"	php_info_print_box_end();
";

		if (isset($this->internal_functions['MINFO'])) {
      $code .= "\n\t{\n";
			$code .= $this->internal_functions['MINFO']->code;
      $code .= "\n\t}\n";
		} else {
       $code .= "\t/* add your stuff here */\n";
    }


if (count($this->phpini)) {
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

    foreach ($this->functions as $function) {
      $code .= $function->c_code(&$this);
    }
		
		return $code;
	}

	// }}} 


  // {{{ code file

	function write_code_file() {
      $filename = "{$this->name}.{$this->language}";  // todo extension logic

			$this->files["c"][] = $filename; 
			$fp = fopen("{$this->name}/$filename", "w");

			$upname = strtoupper($this->name);
			
			fputs($fp, $this->get_license());

			fputs($fp, "#include \"php_{$this->name}.h\"\n\n");
						
      if (isset($this->logo)) {
			  fputs($fp, $this->logo->c_code());	
      }

			if (!empty($this->globals)) {
				fputs($fp, "ZEND_DECLARE_MODULE_GLOBALS({$this->name})\n\n");
			}

			fputs($fp, $this->private_functions_c());

			if (!empty($this->resources)) {
				foreach ($this->resources as $resource) {
					fputs($fp, $resource->c_code());
				}
			}

			fputs($fp, "/* {{{ {$this->name}_functions[] */\n");
			fputs($fp, "function_entry {$this->name}_functions[] = {\n");
			foreach ($this->functions as $name => $function) {
				fputs($fp, sprintf("\tPHP_FE(%-20s, NULL)\n",$name));
			}
			fputs($fp, "\t{ NULL, NULL, NULL }\n");
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


	// {{{ config.m4 file
	function write_config_m4() {
		$upname = strtoupper($this->name);
		
		ob_start();

		echo 
'dnl
dnl $ Id: $
dnl
';
		
		if (isset($this->with['attr'])) {
			echo " 
PHP_ARG_WITH({$this->name}, whether to enable {$this->name} functions,
[  --with-{$this->name}[=DIR]      With {$this->name} support], yes)
";

		echo "
if test \"\$PHP_$upname\" != \"no\"; then
  if test -r \"\$PHP_$upname/{$this->with['attr']['testfile']}\"; then
    PHP_{$upname}_DIR=\"\$PHP_$upname\"
  else
    AC_MSG_CHECKING(for {$this->name} in default path)
    for i in ".str_replace(":"," ",$this->with['attr']['defaults'])."; do
      if test -r \"\$i/{$this->with['attr']['testfile']}\"; then
        PHP_{$upname}_DIR=\$i
        AC_MSG_RESULT(found in \$i)
      fi
    done
  fi
fi

PHP_ADD_INCLUDE(\$PHP_{$upname}_DIR/include)
";

		} else {
		  echo "
PHP_ARG_ENABLE({$this->name} , whether to enable {$this->name} functions,
[  --disable-{$this->name}         Disable {$this->name} support], yes)
";
		}

    echo "\n";


		if (count($this->libs)) {
      $first = true;

			foreach ($this->libs as $lib) {
				if (is_array($lib['platform']) && !in_array("unix", $lib['platform'])) {
					continue;
				}

        if ($first) {
          echo "PHP_SUBST({$upname}_SHARED_LIBADD)\n\n";
          $first = false;
        }

		    if (isset($this->with['attr'])) {
          echo "PHP_ADD_LIBRARY_WITH_PATH($lib[name], \$PHP_{$upname}_DIR/lib, {$upname}_SHARED_LIBADD)\n";
        } else {
          echo "PHP_ADD_LIBRARY($lib[name],, {$upname}_SHARED_LIBADD)\n";
        }

			  if (isset($lib['function'])) {
           echo "AC_CHECK_LIB($lib[name], $lib[function], [AC_DEFINE(HAVE_".strtoupper($lib['name']).",1,[ ])], [AC_MSG_ERROR($lib[name] library not found or wrong version)],)\n";
        }
			}
    }

    echo "\n";

		if ($this->language === "cpp") {
			echo "PHP_REQUIRE_CXX\n";
			echo "PHP_ADD_LIBRARY(stdc++)\n";
		}

		echo "
if test \"\$PHP_$upname\" != \"no\"; then
  AC_DEFINE(HAVE_$upname, 1, [ ])
  PHP_NEW_EXTENSION({$this->name}, ".join(" ", $this->files['c'])." , \$ext_shared)
fi

";

		$filename = "{$this->name}/config.m4";
		$this->files['conf'][] = basename($filename);

		$fp = fopen($filename, "w");
		fputs($fp, ob_get_contents());
		fclose($fp);
    ob_end_clean();
	}

	// }}} 

	// {{{ M$ dev studio project file

	function write_ms_devstudio_dsp() {
    ob_start();

    // these system libraries are always needed?
    // (list taken from sample *.dsp files in php ext tree...)
    $winlibs = "kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ";
    $winlibs.= "shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib";

    // add libraries from <deps> section
    if (count($this->libs)) {
      foreach ($this->libs as $lib) {
				if (is_array($lib['platform']) && !in_array("win32", $lib['platform'])) {
					continue;
				}
        $winlibs .= " $lib[name].lib";
      }
    }

		echo
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
# ADD BASE LINK32 '.$winlibs.' /nologo /dll /machine:I386
# ADD LINK32 php4ts.lib '.$winlibs.' /nologo /dll /machine:I386 /out:"..\..\Release_TS\php_'.$this->name.'.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

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
# ADD BASE LINK32 '.$winlibs.' /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php4ts_debug.lib '.$winlibs.' /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS\php_'.$this->name.'.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"

!ENDIF 

# Begin Target

# Name "'.$this->name.' - Win32 Release_TS"
# Name "'.$this->name.' - Win32 Debug_TS"
';


		echo '
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
';

		foreach ($this->files['c'] as $filename) {
			if ($filename{0}!='/' && $filename{0}!='.') {
				$filename = "./$filename";
			}
			$filename = str_replace("/","\\",$filename);

		 echo "
# Begin Source File

SOURCE=$filename
# End Source File
";
		}

echo '
# End Group
';




echo '
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
';

		foreach ($this->files['h'] as $filename) {
			if ($filename{0}!='/' && $filename{0}!='.') {
				$filename = "./$filename";
			}
			$filename = str_replace("/","\\",$filename);

			echo "
# Begin Source File

SOURCE=$filename
# End Source File
";
		}

echo
'# End Group
# End Target
# End Project
';

    $filename = "{$this->name}/{$this->name}.dsp"; 
		$this->files['conf'][] = basename($filename);

    // write file, enforce DOS/Windows line endings on all platforms
		$fp = fopen($filename,"wb");
    fputs($fp, str_replace("\n","\r\n",ob_get_contents()));
    fclose($fp);
    ob_end_clean();
	}

// }}} 


	function write_credits() {
	  if (count($this->users)) {
       $this->files['doc'][] = "CREDITS";
       $fp = fopen("{$this->name}/CREDITS", "w");
       fputs($fp, "{$this->name}\n");
       $names = array();
       foreach($this->users as $user) {
         if (isset($user['name'])) {
           $names[] = $user['name'];
         }
       }
       fclose($fp);
    }
  }


  function write_experimental() {
    if (isset($this->release['state']) && $this->release['state'] !== 'stable') {
      $this->files['doc'][] = "EXPERIMENTAL";
      $fp = fopen("{$this->name}/EXPERIMENTAL", "w");
      fputs($fp,
"this extension is experimental,
its functions may change their names 
or move to extension all together 
so do not rely to much on them 
you have been warned!
");
      fclose($fp);
    }
  }

	function write_package_xml() {
		$status = false;

		ob_start();
		
		echo 
"<?xml version=\"1.0\" encoding=\"utf-8\">
<!DOCTYPE package SYSTEM \"../../package.dtd\">
<package>
  <name>{$this->name}</name>
";

		if (isset($this->summary)) {
			echo "  <summary>{$this->summary}</summary>\n";
		}

		if (isset($this->description)) {
			echo "  <description>\n".rtrim($this->description)."\n  </description>\n";
		}
		
		if (@is_array($this->users)) {
			echo "\n  <maintainers>\n";
			foreach ($this->users as $user) {
				echo "    <maintainer>\n";
			  foreach (array("user","name","email","role") as $key) {
					if (isset($user[$key])) {
						echo "      <$key>{$user[$key]}</$key>\n";
					}
				}
				echo "    <maintainer/>\n";
			}
			echo "  <maintainers/>\n";
		}
		
		if (is_array($this->release)) {
			echo "\n  <release>\n";
			foreach (array("version","date","state","notes") as $key) {
				if (isset($this->release[$key])) {
					echo "    <$key>{$this->release[$key]}</$key>\n";
				}
			}
			echo "  <release/>\n";
		}

		echo "\n  <filelist>\n";
		echo "    <dir role=\"doc\" name=\"/\">\n";
		if (@is_array($this->files['doc'])) {
			foreach ($this->files['doc'] as $file) {
				echo "      <file role=\"doc\">$file</file>\n";
			}
		}
		if (@is_array($this->files['conf'])) {
			foreach ($this->files['conf'] as $file) {
				echo "      <file role=\"src\">$file</file>\n";
			}
		}
		if (@is_array($this->files['c'])) {
			foreach ($this->files['c'] as $file) {
				echo "      <file role=\"src\">$file</file>\n";
			}
		}
		if (@is_array($this->files['h'])) {
			foreach ($this->files['h'] as $file) {
				echo "      <file role=\"src\">$file</file>\n";
			}
		}
		
		echo "    </dir>\n";
		echo "  </filelist>\n";


		echo "</package>\n";
		
		$fp = fopen("{$this->name}/package.xml", "w");
		if (is_resource($fp)) {
			fputs($fp, ob_get_contents());
			fclose($fp);
		}
		
		ob_end_clean();
		return $status;
	}

	  // }}}

  function write_test_files() {
    mkdir("$this->name/tests");
    
    foreach ($this->functions as $function) {
      $function->write_test($this);
    }
  }

}	

?>