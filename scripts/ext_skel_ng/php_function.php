<?php

	class php_function extends php_element {
	  // all known php types
		function php_function($name, $summary, $proto, $desc="", $code="", $role="") {
			$this->name = $name;
			$this->summary = $summary;
			$this->desc = empty($desc) ? "&warn.undocumented.func;" : $desc;
      $this->code = $code;
			$this->role = empty($role) ? "public" : $role;
			if($this->role === "public") $this->parse_proto($proto);
		} 
		
		function parse_proto($proto) {
			// 'tokenize' it
			$len=strlen($proto);
			$name="";
			$tokens=array();
			for($n=0;$n<$len;$n++) {
				$char = $proto{$n};
				if(ereg("[a-zA-Z0-9_]",$char)) {
					$name.=$char;
				} else {
					if($name) $tokens[]=$name;
					$name="";
					if(trim($char)) $tokens[]=$char;
				}
			}
			if($name) $tokens[]=$name;
			
			$n=0;
			$opts=0;
			$params=array();
			$return_type = ($this->is_type($tokens[$n])) ? $tokens[$n++] : "void";
			if(! $this->is_name($tokens[$n])) die("$tokens[$n] is not a valid function name");
			$function_name = $tokens[$n++];
			if($tokens[$n]!='(') die("'(' expected instead of '$tokens[$n]'");
			if($tokens[++$n]!=')') {			
				for($param=0;$tokens[$n];$n++,$param++) {
					if($tokens[$n]=='[') {
						$params[$param]['optional']=true;
						$opts++;
						$n++;
						if($param>0) { 
							if ($tokens[$n]!=',') die("',' expected after '[' instead of $token[$n]");
							$n++;
						}
					}
					if(!$this->is_type($tokens[$n])) die("type name expected instead of $tokens[$n]");
					$params[$param]['type']=$tokens[$n];
					$n++;
					if($this->is_name($tokens[$n])) {
						$params[$param]['name']=$tokens[$n];
						$n++;
					}
					if($tokens[$n]=='[') {
						$n--;
						continue;
					}
					if($tokens[$n]==',') continue;
					if($tokens[$n]==']') break;
					if($tokens[$n]==')') break;			
				}
			}
			$numopts=$opts;
			while($tokens[$n]==']') {
				$n++;
				$opts--;
			}
			if($opts!=0) die ("'[' / ']' count mismatch");
			if($tokens[$n] != ')') die ("')' expected instead of $tokens[$n]");
			
			$this->name     = $function_name;
			$this->returns  = $return_type;
			$this->params   = $params;
			$this->optional = $numopts;
		}

		function c_code() {
			$code = "";

			switch($this->role) {
			case "public":
			$code .= "\n/* {{{ proto {$this->returns} {$this->name}(";
			if(isset($this->params)) {
				foreach($this->params as $param) {
					if(!empty($param['optional']))
						$code.=" [";
					if($key) 
						$code.=", ";
					$code .= $param['type']." ";
					$code .= $param['name']; 
				}
			}
			for($n=$this->optional; $n>0; $n--) {
				$code .= "]";
			}
			$code .= ")\n  ";
			if(!empty($this->summary)) {
				$code .= $this->summary;
			}
			$code .= " */\n";
			$code .= "PHP_FUNCTION({$this->name})\n";
			$code .= "{\n";
			$code .= "\tint argc = ZEND_NUM_ARGS();\n\n";
			if(isset($this->params)) {
				$arg_string="";
				$arg_pointers=array();
				$optional=false;
				$res_fetch="";
				foreach($this->params as $param) {
					$name = $param['name']; 
					$arg_pointers[]="&$name";
					if(isset($param['optional'])&&!$optional) {
						$optional=true;
						$arg_string.="|";
					}
					switch($param['type']) {
						//case "void":
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
				$code .= "\tif(argc>0) { WRONG_PARAM_COUNT; }\n\n";
			}
			$code .= "\tphp_error(E_WARNING, \"{$this->name}: not yet implemented\");\n";
			$code .= "}\n/* }}} */\n\n";
			  break;
		  case "internal":
				if(!empty($this->code)) {
					$code .= "\t\t{\n";
					$code .= $this->code."\n";
					$code .= "\t\t}\n";
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
<!-- $Revision$ -->
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
			if(empty($this->params)) {
				$xml .= "      <void/>\n";
			} else {
				foreach($this->params as $key => $param) {
					if(isset($param['optional'])) {
						$xml .= "      <methodparam choice='opt'>";
					} else {
						$xml .= "      <methodparam>";
					}
					$xml .= "<type>$param[type]</type><parameter>$param[name]</parameter>";
					$xml .= "</methodparam>\n";
				}
			}

$xml .= 
'     </methodsynopsis>
    <para>
'.$this->desc.'
    </para>
   </refsect1>
  </refentry>
';
      $xml .= $this->docbook_editor_footer(4);
 
			return $xml;
		}
	}

?>