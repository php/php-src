<?php
	class xml_stream_callback_parser extends xml_stream_parser {
		function xml_stream_callback_parser ($stream) {
			$this->cdata = "";
			$this->tags = array();
			$this->attrs = array();

			parent::xml_stream_parser($stream);
		}

		function cdata($parser, $cdata) {
			$this->cdata .= $cdata;
		}

		function tag_open($parser, $tag, $attributes) {
			array_push($this->tags, $tag); 
			array_push($this->attrs, $attributes); 
		}

		function tag_close($parser, $tag) {
			$attributes = array_pop($this->attrs);

			for ($tags = $this->tags; count($tags); array_shift($tags)) {
				$method = "handle_".join("_", $tags);
				if(method_exists($this, $method)) {
					$this->$method($attributes);
					break;
				}
			}
		 
			$this->cdata = "";
			array_pop($this->tags); 
		}
	}
?>