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
  print "Node Name: " . $node->nodeName;
  print "\nNode Type: " . $node->nodeType;
  if ($node->nodeType != 3) {
      $child_count = $node->childNodes->length;
  } else {
      $child_count = 0;
  }
  print "\nNum Children: " . $child_count;
  if($child_count <= 1){
    if (strlen(trim($node->nodeValue))) {
      print "\nNode Content: " . $node->nodeValue;
    } else {
      print "\nNode Content:";
    }
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

function print_node_compact($node, $spaces)
{
    if ($node->nodeType == 3) {
        print str_repeat(" ", $spaces) . trim($node->nodeValue) . "\n";
    } else {
        print str_repeat(" ", $spaces) . "<" . $node->nodeName . ">\n";
        print_node_list_compact($node->childNodes, $spaces + 2);
        print str_repeat(" ", $spaces) . "</" . $node->nodeName . ">\n";
    }
}

function print_node_list_compact($nodelist, $spaces = 0)
{
  foreach ($nodelist as $node) {
    print_node_compact($node, $spaces);
  }
}

?>
