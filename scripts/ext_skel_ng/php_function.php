<?php

	class php_function extends php_element {

		function __construct($name, $summary, $proto, $desc="", $code="", $role="") {
			$this->name = $name;
			$this->summary = $summary;
			$this->desc = empty($desc) ? "&warn.undocumented.func;" : $desc;
      $this->code = $code;
			$this->role = empty($role) ? "public" : $role;
			if ($this->role === "public") {
				$this->status = $this->parse_proto($proto);
			}
		} 
		
		function parse_proto($proto) {
			// 'tokenize' it
			$len=strlen($proto);
			$name="";
			$tokens=array();
			for ($n=0;$n<$len;$n++) {
				$char = $proto{$n};
				if (ereg("[a-zA-Z0-9_]",$char)) {
					$name.=$char;
				} else {
					if ($name) $tokens[]=$name;
					$name="";
					if (trim($char)) $tokens[]=$char;
				}
			}
			if ($name) $tokens[]=$name;
			
			$n=0;
			$opts=0;
			$params=array();
			$return_type = ($this->is_type($tokens[$n])) ? $tokens[$n++] : "void";
			$function_name = $tokens[$n++];
			if ($return_type === "resource" && $tokens[$n] !== "(") {
				$return_subtype = $function_name;
				$function_name = $tokens[$n++];
			}
			if (! $this->is_name($function_name)) {
				return("$function_name is not a valid function name");
			}
			if ($function_name != $this->name) {
				return "proto function name is '$function_name' instead of '{$this->name}'";
			}

			if ($tokens[$n]!='(') return("'(' expected instead of '$tokens[$n]'");
			if ($tokens[++$n]!=')') {			
				for ($param=0;$tokens[$n];$n++,$param++) {
					if ($tokens[$n]=='[') {
						$params[$param]['optional']=true;
						$opts++;
						$n++;
						if ($param>0) { 
							if ($tokens[$n]!=',') return("',' expected after '[' instead of '$token[$n]'");
							$n++;
						}
					}
					if (!$this->is_type($tokens[$n])) return("type name expected instead of '$tokens[$n]'");
					$params[$param]['type']=$tokens[$n];
					$n++;
					if ($tokens[$n] == "&") {
						$params[$param]['by_ref'] = true;
						$n++;
					}
					if ($this->is_name($tokens[$n])) {
						$params[$param]['name']=$tokens[$n];
						$n++;
					}
					if ($tokens[$n] == "&") {
						$params[$param]['by_ref'] = true;
						$n++;
					}
					if ($params[$param]['type'] === "resource" && $this->is_name($tokens[$n])) {
						$params[$param]['subtype'] = $params[$param]['name'];
						$params[$param]['name'] = $tokens[$n];
						$n++;
					}
					if ($tokens[$n]=='[') {
						$n--;
						continue;
					}
					if ($tokens[$n]==',') continue;
					if ($tokens[$n]==']') break;
					if ($tokens[$n]==')') break;			
				}
			}
			$numopts=$opts;
			while ($tokens[$n]==']') {
				$n++;
				$opts--;
			}
			if ($opts!=0) return ("'[' / ']' count mismatch");
			if ($tokens[$n] != ')') return("')' expected instead of '$tokens[$n]'");
			
			$this->name     = $function_name;
			$this->returns  = $return_type;
			if (isset($return_subtype)) {
				$this->returns .= " $return_subtype";
			}
			$this->params   = $params;
			$this->optional = $numopts;

			return true;
		}

		function c_code($extension) {
			$code = "";

			$returns = explode(" ", $this->returns);

			switch ($this->role) {
			case "public":
			$code .= "\n/* {{{ proto {$this->returns} {$this->name}(";
			if (isset($this->params)) {
				foreach ($this->params as $key => $param) {
					if (!empty($param['optional']))
						$code.=" [";
					if ($key) 
						$code.=", ";
					$code .= $param['type']." ";
					if (isset($param['subtype'])) {
						$code .= $param['subtype']." ";
					}
					if ($param['type'] !== 'void') {
						if (isset($param['by_ref'])) {
							$code .= "&";
						}
					  $code .= $param['name']; 
          }
				}
			}
			for ($n=$this->optional; $n>0; $n--) {
				$code .= "]";
			}
			$code .= ")\n  ";
			if (!empty($this->summary)) {
				$code .= $this->summary;
			}
			$code .= " */\n";
			$code .= "PHP_FUNCTION({$this->name})\n";
			$code .= "{\n";

			if ($returns[0] === "resource" && isset($returns[1])) {
				$resource = $extension->resources[$returns[1]];
				if ($resource->alloc === "yes") {
					$payload  = $resource->payload;
					$code .= "  $payload * return_res = ($payload *)emalloc(sizeof($payload));\n";
				}
			}

			if (isset($this->params) && count($this->params)) {
				$arg_string="";
				$arg_pointers=array();
				$optional=false;
				$res_fetch="";
				foreach ($this->params as $key => $param) {
          if ($param["type"] === "void") continue;
					$name = $param['name']; 
					$arg_pointers[]="&$name";
					if (isset($param['optional'])&&!$optional) {
						$optional=true;
						$arg_string.="|";
					}
					switch ($param['type']) {
					case "bool":
						$arg_string.="b";
						$code .= "  zend_bool $name = 0;\n";
						break;
					case "int":
						$arg_string.="l";
						$code .= "  long $name = 0;\n";
						break;
					case "float":
						$arg_string.="d";
						$code .= "  double $name = 0.0;\n";
						break;
					case "string":
						$arg_string.="s";
						$code .= "  char * $name = NULL;\n";
						$code .= "  int {$name}_len = 0;\n";
						$arg_pointers[]="&{$name}_len";
						break;
					case "array":
						$arg_string.="a";
						$code .= "  zval * $name = NULL;\n";
						break;
					case "object": 
						$arg_string.="o";
						$code .= "  zval * $name = NULL;\n";
						break;
					case "resource":
						$arg_string.="r";
						$code .= "  zval * $name = NULL;\n";
						$code .= "  int {$name}_id = -1;\n";
						// dummfug?						$arg_pointers[]="&{$name}_id";
						if (isset($param['subtype'])) {
							$resource = $extension->resources[$param['subtype']];
							$varname = "res_{$name}";
							$code .= "  {$resource->payload} * $varname;\n";

							$res_fetch.="  if ($name) {\n"
								."    ZEND_FETCH_RESOURCE($varname, {$resource->payload} *, &$name, {$name}_id, \"$param[subtype]\", le_$param[subtype]);\n"
								."  }\n";
						} else {
							$res_fetch.="  if ($name) {\n"
								."    ZEND_FETCH_RESOURCE(???, ???, $name, {$name}_id, \"???\", ???_rsrc_id);\n"
								."  }\n";
						}
						break;
					case "stream":
						$arg_string .= "r";
						$code .= "  zval * _z$name = NULL; \n";
						$code .= "  php_stream * $name = NULL:\n";
						$res_fetch.= "  php_stream_from_zval($name, &_z$name);\n"; 
						break;
					case "mixed":
					case "callback":
						$arg_string.="z";
						$code .= "  zval * $name = NULL;\n";
						break;
					}
				}
			} 

			if (isset($arg_string) && strlen($arg_string)) {
				$code .= "  int argc = ZEND_NUM_ARGS();\n\n";
				$code .= "  if (zend_parse_parameters(argc TSRMLS_CC, \"$arg_string\", ".join(", ",$arg_pointers).") == FAILURE) return;\n";
						if ($res_fetch) $code.="\n$res_fetch\n";
			} else {
				$code .= "  if (ZEND_NUM_ARGS()>0) { WRONG_PARAM_COUNT; }\n\n";
			}

			if ($this->code) {
				$code .= "  {\n$this->code  }\n"; // {...} for local variables ...
			} else {
				$code .= "  php_error(E_WARNING, \"{$this->name}: not yet implemented\"); RETURN_FALSE;\n\n";

				switch ($returns[0]) {
				case "void":
					break;
				
				case "bool":
					$code .= "  RETURN_FALSE;\n"; 
					break;
				
				case "int":
					$code .= "  RETURN_LONG(0);\n"; 
					break;
				
				case "float":
					$code .= "  RETURN_DOUBLE(0.0);\n";
					break;
				
				case "string":
					$code .= "  RETURN_STRINGL(\"\", 0, 1);\n";
					break;
				
				case "array":
					$code .= "  array_init(return_value);\n";
					break;
				
				case "object": 
					$code .= "  object_init(return_value)\n";
					break;
				
				case "resource":
					if (isset($returns[1])) {
						$code .= "  ZEND_REGISTER_RESOURCE(return_value, return_res, le_$returns[1]);\n";
					} else {
						$code .= "  /* RETURN_RESOURCE(...); /*\n";
					}
					break;

				case "stream":
					$code .= "  /* php_stream_to_zval(stream, return_value); */\n";
					break;
				
				case "mixed":
					$code .= "  /* RETURN_...(...); /*\n";				
					break;

				default: 
					$code .= "  /* UNKNOWN RETURN TYPE '$this->returns' /*\n";
					break;
				}
			}
			
			$code .= "}\n/* }}} */\n\n";
			break;
		  
			case "internal":
				if (!empty($this->code)) {
					$code .= "    {\n";
					$code .= $this->code."\n";
					$code .= "    }\n";
				}
				break;
  		
			case "private":
				$code .= $this->code."\n";				
				break;
		  }
			return $code;
		}

		function docbook_xml() {
			$xml = 
'<?xml version="1.0" encoding="iso-8859-1"?>
<!-- $Rev'.'ision: 1.0 $ -->
  <refentry id="function.'.strtolower(str_replace("_","-",$this->name)).'">
   <refnamediv>
    <refname>'.$this->name.'</refname>
    <refpurpose>'.$this->summary.'</refpurpose>
   </refnamediv>
   <refsect1>
    <title>Description</title>
     <methodsynopsis>
';

			$xml .= "      <type>{$this->returns}</type><methodname>{$this->name}</methodname>\n";
			if (empty($this->params) || $this->params[0]["type"]==="void") {
				$xml .= "      <void/>\n";
			} else {
				foreach ($this->params as $key => $param) {
					if (isset($param['optional'])) {
						$xml .= "      <methodparam choice='opt'>";
					} else {
						$xml .= "      <methodparam>";
					}
					$xml .= "<type>$param[type]</type><parameter>";
					if (isset($param['by_ref'])) {
						$xml .= "&amp;"; 
					}
					$xml .= "$param[name]</parameter>";
					$xml .= "</methodparam>\n";
				}
			}


			$desc = $this->desc;

			if (!strstr($this->desc,"<para>")) {
				$desc = "     <para>\n$desc     </para>\n";
			}

$xml .= 
'     </methodsynopsis>
'.$desc.'
   </refsect1>
  </refentry>
';
      $xml .= $this->docbook_editor_settings(4);
 
			return $xml;
		}

		function write_test($extension) {
			$fp = fopen("{$extension->name}/tests/{$this->name}.phpt", "w");
			fputs($fp, 
"--TEST--
{$this->name}() function
--SKIPIF--
<?php if (!extension_loaded('{$extension->name}')) print 'skip'; ?>
--POST--
--GET--
--FILE--
<?php 
	echo 'no test case for {$this->name}() yet';
?>
--EXPECT--
no test case for {$this->name}() yet");
			fclose($fp);
		}
	}

?>