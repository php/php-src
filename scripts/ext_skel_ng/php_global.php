<?php

	class php_global extends php_element {

		function __construct($attr) {
			
			$this->name  = $attr["name"];
	
			if (!$this->is_name($this->name)) {
				$this->error[] = "'$attr[name]' is not a valid C variable name";
			}

			$this->type  = $attr['type'];
      if (!$this->is_type($this->type)) {
        $this->error[] = "'$attr[type]' is not a valid C data type";
      }
		} 

		/* overriding type check as we deal with C types here
			 check is rather naive as it doesn't know about context
			 so we check for a sequence of valid names for now
			 TODO: check for either simple type, struct/class or single word (typedef)
		*/
		function is_type($type) {
			$array = explode(" ", str_replace('*',' ',$type));
			foreach ($array as $name) {
				if (empty($name)) continue;
				if (!$this->is_name($name)) return false;
			}
			return true;
		}
		


		static function c_code_header($name) {
			return "static void php_{name}_init_globals(zend_{name}_globals *{name}_globals)\n{\n";
		}

		function c_code($name) {
			$code = "  {$name}_globals->{$this->name} = ";
			if (strstr($this->type, "*")) {
				$code .= "NULL;\n";
			} else {
				$code .= "0;\n";
			}
			return $code;
		}

		static function c_code_footer() {
			return "}\n\n";
		}

		static function h_code_header($name) {
			return "ZEND_BEGIN_MODULE_GLOBALS({$name})\n";
		}

		function h_code($name) {
			return "  {$this->type} {$this->name};\n";
		}

		static function h_code_footer($name) {
			$upname = strtoupper($name);

			return "
ZEND_END_MODULE_GLOBALS({$name})

#ifdef ZTS
#define {$upname}_G(v) TSRMG({$name}_globals_id, zend_{$name}_globals *, v)
#else
#define {$upname}_G(v) ({$name}_globals.v)
#endif

";
			
		}


	}

?>
