<?php
class xml_stream_parser  {
    var $parser;

    function xml_stream_parser($stream)
    {
			if(!is_resource($stream)) die("not a stream");
			if(get_resource_type($stream) != "stream") die("not a stream");

			$this->parser = xml_parser_create();
			
			xml_set_object($this->parser, $this);
			xml_set_element_handler($this->parser, "tag_open", "tag_close");
			xml_set_character_data_handler($this->parser, "cdata");
			xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, false);
		
			while(!feof($stream)) {
				xml_parse($this->parser, fgets($stream), feof($stream));
			}	
			xml_parser_free($this->parser);
    }

    function tag_open($parser, $tag, $attributes)
    { 
        var_dump($parser, $tag, $attributes); 
    }

    function cdata($parser, $cdata)
    {
        var_dump($parser, $cdata);
    }

    function tag_close($parser, $tag)
    {
        var_dump($parser, $tag);
    }

		function error($msg) {
			die("$msg on line ".xml_get_current_line_number($this->parser));
		}
} // end of class xml
?>