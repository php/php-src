<?PHP
$xmlstr = "<?xml version='1.0' standalone='yes'?>
<!DOCTYPE chapter SYSTEM '/share/sgml/Norman_Walsh/db3xml10/db3xml10.dtd'
[ <!ENTITY sp \"spanish\">
]>
<!-- lsfj  -->
<chapter language='en'><title language='en'>Title</title>
<para language='ge'>
&sp;
<!-- comment -->
<informaltable language='&sp;kkk'>
<tgroup cols='3'>
<tbody>
<row><entry>a1</entry><entry morerows='1'>b1</entry><entry>c1</entry></row>
<row><entry>a2</entry><entry>c2</entry></row>
<row><entry>a3</entry><entry>b3</entry><entry>c3</entry></row>
</tbody>
</tgroup>
</informaltable>
</para>
</chapter> ";

function print_node($node)
{
  print "Node Name: " . $node->node_name();
  print "\nNode Type: " . $node->node_type();
  print "\nNum Children: " . count($node->children());
  if(count($node->children()) <= 1){
    print "\nNode Content: " . $node->get_content();
  }
  print "\n\n";
}

function print_node_list($nodelist)
{
  foreach($nodelist as $node)
  {
    print_node($node);
  }
}

?>
