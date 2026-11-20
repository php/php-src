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
--EXPECTF--
Deprecated: Implicit conversion from BadStr to string for property DOMDocument::$encoding of type ?string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMDocument::$version of type ?string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMDocument::$documentURI of type ?string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMAttr::$value of type string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMNode::$nodeValue of type ?string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMNode::$nodeValue of type ?string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMNode::$textContent of type string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMCharacterData::$data of type string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMNode::$prefix of type string is deprecated in %s on line %d
Exception

Deprecated: Implicit conversion from BadStr to string for property DOMProcessingInstruction::$data of type string is deprecated in %s on line %d
Exception
<?xml version="1.0"?>
<root xmlns:ns="foo"><node attr="foo"/><node>Text</node><ns:node/><?pi foobar?></root>
