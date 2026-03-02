--TEST--
Handling of exceptions during __toString
--EXTENSIONS--
dom
--FILE--
<?php

class BadStr {
    public function __toString() {
        throw new Exception("Exception");
    }
}

$badStr = new BadStr;

$doc = new DOMDocument();
$doc->loadXML(
    '<root xmlns:ns="foo"><node attr="foo" /><node>Text</node><ns:node/><?pi foobar?></root>');

try { $doc->encoding = $badStr; } catch (Exception $e) { echo "Exception\n"; }
try { $doc->version = $badStr; } catch (Exception $e) { echo "Exception\n"; }
try { $doc->documentURI = $badStr; } catch (Exception $e) { echo "Exception\n"; }
$root = $doc->childNodes[0];

$node = $root->childNodes[0];
$attrs = $node->attributes;
$attr = $attrs[0];
try { $attr->value = $badStr; } catch (Exception $e) { echo "Exception\n"; }
try { $attr->nodeValue = $badStr; } catch (Exception $e) { echo "Exception\n"; }

$node2 = $root->childNodes[1];
try { $node2->nodeValue = $badStr; } catch (Exception $e) { echo "Exception\n"; }
try { $node2->textContent = $badStr; } catch (Exception $e) { echo "Exception\n"; }
$data = $node2->childNodes[0];
try { $data->data = $badStr; } catch (Exception $e) { echo "Exception\n"; }

$node3 = $root->childNodes[2];
try { $node3->prefix = $badStr; } catch (Exception $e) { echo "Exception\n"; }

$pi = $root->childNodes[3];
try { $pi->data = $badStr; } catch (Exception $e) { echo "Exception\n"; }

echo $doc->saveXML();

?>
--EXPECT--
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
Exception
<?xml version="1.0"?>
<root xmlns:ns="foo"><node attr="foo"/><node>Text</node><ns:node/><?pi foobar?></root>
