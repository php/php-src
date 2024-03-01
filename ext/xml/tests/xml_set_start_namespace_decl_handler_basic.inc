<?php
$xml = <<<HERE
<aw1:book xmlns:aw1="http://www.somewhere.com/namespace1"
          xmlns:aw2="file:/DTD/somewhere.dtd">
<aw1:para>Any old text.</aw1:para>
<aw2:td>An HTML table cell.</aw2:td>
</aw1:book>
HERE;

$parser = xml_parser_create_ns();
xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, 0);

var_dump(xml_set_start_namespace_decl_handler( $parser, "Namespace_Start_Handler" ));
var_dump(xml_set_end_namespace_decl_handler( $parser, "Namespace_End_Handler" ));

xml_parse( $parser, $xml, true);
xml_parser_free( $parser );

echo "Done\n";

function Namespace_Start_Handler( $parser, $prefix, $uri ) {
    echo "Namespace_Start_Handler called\n";
    echo "...Prefix: ". $prefix . "\n";
    echo "...Uri: ". $uri . "\n";
}

function Namespace_End_Handler($parser, $prefix) {
    echo "Namespace_End_Handler called\n";
    echo "...Prefix: ". $prefix . "\n\n";
}

function DefaultHandler( $parser, $data ) {
   print( 'DefaultHandler Called<br/>' );
}