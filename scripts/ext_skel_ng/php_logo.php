<?php

	class php_logo extends php_element {
		function php_logo($name, $attr) {
			$this->name = $name;
			$this->attr = $attr;
			$this->id = '"'.strtoupper($name).'_LOGO_ID"';

			$this->data = file_get_contents($attr['src']);
			$this->size = strlen($this->data);

			$this->mime_type = "image/gif";
		} 
		
		function docbook_xml($base) {
			return "";
		}

		function minit_code() {
			return "  php_register_info_logo({$this->id}, \"{$this->mime_type}\", {$this->name}_logo, {$this->size});\n";
		}

		function c_code() {
			return "
static unsigned char {$this->name}_logo[] = {
#include \"{$this->name}_logo.h\"
}; 
";
		}

		function h_code() {
			$len = strlen($this->data);
			$code = " ";
			$i=0;
			for($n = 0; $n < $len; $n++) {
				$code .= sprintf(" %3d",ord($this->data[$n]));
				if($n == $len - 1) break;
				$code .=  ",";
				if(++$i==8) {
					$code .= "\n ";
					$i=0;
				}
			}
			
			$code .= "\n";
			
			return $code;
		}
	}

?>