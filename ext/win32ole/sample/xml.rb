#
# This file created by olegen.rb as following.
#    ruby olegen.rb 'Microsoft XML, version 2.0' > xml.rb
#
require 'win32ole'
require 'win32ole/property'

#
module IXMLDOMImplementation
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BOOL hasFeature
  #   BSTR arg0 --- feature [IN]
  #   BSTR arg1 --- version [IN]
  def hasFeature(arg0, arg1)
    ret = _invoke(145, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# Core DOM node interface
module IXMLDOMNode
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# Constants that define a node's type
module OLEtagDOMNodeType
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  NODE_INVALID = 0
  NODE_ELEMENT = 1
  NODE_ATTRIBUTE = 2
  NODE_TEXT = 3
  NODE_CDATA_SECTION = 4
  NODE_ENTITY_REFERENCE = 5
  NODE_ENTITY = 6
  NODE_PROCESSING_INSTRUCTION = 7
  NODE_COMMENT = 8
  NODE_DOCUMENT = 9
  NODE_DOCUMENT_TYPE = 10
  NODE_DOCUMENT_FRAGMENT = 11
  NODE_NOTATION = 12
end

#
module IXMLDOMNodeList
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # I4 length
  # number of nodes in the collection
  def length()
    ret = _getproperty(74, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # PTR item
  # collection of nodes
  #   I4 arg0 --- index [IN]
  def item
    OLEProperty.new(self, 0, [VT_I4], [VT_I4, VT_BYREF|VT_DISPATCH])
  end

  # IXMLDOMNode nextNode
  # get next node from iterator
  def nextNode()
    ret = _invoke(76, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID reset
  # reset the position of iterator
  def reset()
    ret = _invoke(77, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMNamedNodeMap
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # I4 length
  # number of nodes in the collection
  def length()
    ret = _getproperty(74, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # PTR item
  # collection of nodes
  #   I4 arg0 --- index [IN]
  def item
    OLEProperty.new(self, 0, [VT_I4], [VT_I4, VT_BYREF|VT_DISPATCH])
  end

  # IXMLDOMNode getNamedItem
  # lookup item by name
  #   BSTR arg0 --- name [IN]
  def getNamedItem(arg0)
    ret = _invoke(83, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode setNamedItem
  # set item by name
  #   IXMLDOMNode arg0 --- newItem [IN]
  def setNamedItem(arg0)
    ret = _invoke(84, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeNamedItem
  # remove item by name
  #   BSTR arg0 --- name [IN]
  def removeNamedItem(arg0)
    ret = _invoke(85, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode getQualifiedItem
  # lookup the item by name and namespace
  #   BSTR arg0 --- baseName [IN]
  #   BSTR arg1 --- namespaceURI [IN]
  def getQualifiedItem(arg0, arg1)
    ret = _invoke(87, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeQualifiedItem
  # remove the item by name and namespace
  #   BSTR arg0 --- baseName [IN]
  #   BSTR arg1 --- namespaceURI [IN]
  def removeQualifiedItem(arg0, arg1)
    ret = _invoke(88, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextNode
  # get next node from iterator
  def nextNode()
    ret = _invoke(89, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID reset
  # reset the position of iterator
  def reset()
    ret = _invoke(90, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMDocument
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocumentType doctype
  # node corresponding to the DOCTYPE
  def doctype()
    ret = _getproperty(38, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMImplementation implementation
  # info on this DOM implementation
  def implementation()
    ret = _getproperty(39, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMElement documentElement
  # the root of the tree
  def documentElement()
    ret = _getproperty(40, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 readyState
  # get the state of the XML document
  def readyState()
    ret = _getproperty(-525, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMParseError parseError
  # get the last parser error
  def parseError()
    ret = _getproperty(59, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR url
  # get the URL for the loaded XML document
  def url()
    ret = _getproperty(60, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL async
  # flag for asynchronous download
  def async()
    ret = _getproperty(61, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL validateOnParse
  # indicates whether the parser performs validation
  def validateOnParse()
    ret = _getproperty(65, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL resolveExternals
  # indicates whether the parser resolves references to external DTD/Entities/Schema
  def resolveExternals()
    ret = _getproperty(66, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL preserveWhiteSpace
  # indicates whether the parser preserves whitespace
  def preserveWhiteSpace()
    ret = _getproperty(67, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID async
  # flag for asynchronous download
  def async=(arg0)
    ret = _setproperty(61, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID validateOnParse
  # indicates whether the parser performs validation
  def validateOnParse=(arg0)
    ret = _setproperty(65, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID resolveExternals
  # indicates whether the parser resolves references to external DTD/Entities/Schema
  def resolveExternals=(arg0)
    ret = _setproperty(66, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID preserveWhiteSpace
  # indicates whether the parser preserves whitespace
  def preserveWhiteSpace=(arg0)
    ret = _setproperty(67, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID onreadystatechange
  # register a readystatechange event handler
  def onreadystatechange=(arg0)
    ret = _setproperty(68, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID ondataavailable
  # register an ondataavailable event handler
  def ondataavailable=(arg0)
    ret = _setproperty(69, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID ontransformnode
  # register an ontransformnode event handler
  def ontransformnode=(arg0)
    ret = _setproperty(70, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMElement createElement
  # create an Element node
  #   BSTR arg0 --- tagName [IN]
  def createElement(arg0)
    ret = _invoke(41, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocumentFragment createDocumentFragment
  # create a DocumentFragment node
  def createDocumentFragment()
    ret = _invoke(42, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMText createTextNode
  # create a text node
  #   BSTR arg0 --- data [IN]
  def createTextNode(arg0)
    ret = _invoke(43, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMComment createComment
  # create a comment node
  #   BSTR arg0 --- data [IN]
  def createComment(arg0)
    ret = _invoke(44, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMCDATASection createCDATASection
  # create a CDATA section node
  #   BSTR arg0 --- data [IN]
  def createCDATASection(arg0)
    ret = _invoke(45, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMProcessingInstruction createProcessingInstruction
  # create a processing instruction node
  #   BSTR arg0 --- target [IN]
  #   BSTR arg1 --- data [IN]
  def createProcessingInstruction(arg0, arg1)
    ret = _invoke(46, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMAttribute createAttribute
  # create an attribute node
  #   BSTR arg0 --- name [IN]
  def createAttribute(arg0)
    ret = _invoke(47, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMEntityReference createEntityReference
  # create an entity reference node
  #   BSTR arg0 --- name [IN]
  def createEntityReference(arg0)
    ret = _invoke(49, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList getElementsByTagName
  # build a list of elements by name
  #   BSTR arg0 --- tagName [IN]
  def getElementsByTagName(arg0)
    ret = _invoke(50, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode createNode
  # create a node of the specified node type and name
  #   VARIANT arg0 --- type [IN]
  #   BSTR arg1 --- name [IN]
  #   BSTR arg2 --- namespaceURI [IN]
  def createNode(arg0, arg1, arg2)
    ret = _invoke(54, [arg0, arg1, arg2], [VT_VARIANT, VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nodeFromID
  # retrieve node from it's ID
  #   BSTR arg0 --- idString [IN]
  def nodeFromID(arg0)
    ret = _invoke(56, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL load
  # load document from the specified XML source
  #   VARIANT arg0 --- xmlSource [IN]
  def load(arg0)
    ret = _invoke(58, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID abort
  # abort an asynchronous download
  def abort()
    ret = _invoke(62, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL loadXML
  # load the document from a string
  #   BSTR arg0 --- bstrXML [IN]
  def loadXML(arg0)
    ret = _invoke(63, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID save
  # save the document to a specified desination
  #   VARIANT arg0 --- desination [IN]
  def save(arg0)
    ret = _invoke(64, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMDocumentType
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR name
  # name of the document type (root of the tree)
  def name()
    ret = _getproperty(131, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap entities
  # a list of entities in the document
  def entities()
    ret = _getproperty(132, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap notations
  # a list of notations in the document
  def notations()
    ret = _getproperty(133, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMElement
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR tagName
  # get the tagName of the element
  def tagName()
    ret = _getproperty(97, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT getAttribute
  # look up the string value of an attribute by name
  #   BSTR arg0 --- name [IN]
  def getAttribute(arg0)
    ret = _invoke(99, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID setAttribute
  # set the string value of an attribute by name
  #   BSTR arg0 --- name [IN]
  #   VARIANT arg1 --- value [IN]
  def setAttribute(arg0, arg1)
    ret = _invoke(100, [arg0, arg1], [VT_BSTR, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID removeAttribute
  # remove an attribute by name
  #   BSTR arg0 --- name [IN]
  def removeAttribute(arg0)
    ret = _invoke(101, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMAttribute getAttributeNode
  # look up the attribute node by name
  #   BSTR arg0 --- name [IN]
  def getAttributeNode(arg0)
    ret = _invoke(102, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMAttribute setAttributeNode
  # set the specified attribute on the element
  #   IXMLDOMAttribute arg0 --- DOMAttribute [IN]
  def setAttributeNode(arg0)
    ret = _invoke(103, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMAttribute removeAttributeNode
  # remove the specified attribute
  #   IXMLDOMAttribute arg0 --- DOMAttribute [IN]
  def removeAttributeNode(arg0)
    ret = _invoke(104, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList getElementsByTagName
  # build a list of elements by name
  #   BSTR arg0 --- tagName [IN]
  def getElementsByTagName(arg0)
    ret = _invoke(105, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID normalize
  # collapse all adjacent text nodes in sub-tree
  def normalize()
    ret = _invoke(106, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMAttribute
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR name
  # get name of the attribute
  def name()
    ret = _getproperty(118, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT value
  # string value of the attribute
  def value()
    ret = _getproperty(120, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID value
  # string value of the attribute
  def value=(arg0)
    ret = _setproperty(120, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMDocumentFragment
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMText
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR data
  # value of the node
  def data()
    ret = _getproperty(109, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 length
  # number of characters in value
  def length()
    ret = _getproperty(110, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID data
  # value of the node
  def data=(arg0)
    ret = _setproperty(109, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR substringData
  # retrieve substring of value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def substringData(arg0, arg1)
    ret = _invoke(111, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID appendData
  # append string to value
  #   BSTR arg0 --- data [IN]
  def appendData(arg0)
    ret = _invoke(112, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID insertData
  # insert string into value
  #   I4 arg0 --- offset [IN]
  #   BSTR arg1 --- data [IN]
  def insertData(arg0, arg1)
    ret = _invoke(113, [arg0, arg1], [VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID deleteData
  # delete string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def deleteData(arg0, arg1)
    ret = _invoke(114, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID replaceData
  # replace string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  #   BSTR arg2 --- data [IN]
  def replaceData(arg0, arg1, arg2)
    ret = _invoke(115, [arg0, arg1, arg2], [VT_I4, VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMText splitText
  # split the text node into two text nodes at the position specified
  #   I4 arg0 --- offset [IN]
  def splitText(arg0)
    ret = _invoke(123, [arg0], [VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMCharacterData
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR data
  # value of the node
  def data()
    ret = _getproperty(109, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 length
  # number of characters in value
  def length()
    ret = _getproperty(110, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID data
  # value of the node
  def data=(arg0)
    ret = _setproperty(109, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR substringData
  # retrieve substring of value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def substringData(arg0, arg1)
    ret = _invoke(111, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID appendData
  # append string to value
  #   BSTR arg0 --- data [IN]
  def appendData(arg0)
    ret = _invoke(112, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID insertData
  # insert string into value
  #   I4 arg0 --- offset [IN]
  #   BSTR arg1 --- data [IN]
  def insertData(arg0, arg1)
    ret = _invoke(113, [arg0, arg1], [VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID deleteData
  # delete string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def deleteData(arg0, arg1)
    ret = _invoke(114, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID replaceData
  # replace string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  #   BSTR arg2 --- data [IN]
  def replaceData(arg0, arg1, arg2)
    ret = _invoke(115, [arg0, arg1, arg2], [VT_I4, VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMComment
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR data
  # value of the node
  def data()
    ret = _getproperty(109, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 length
  # number of characters in value
  def length()
    ret = _getproperty(110, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID data
  # value of the node
  def data=(arg0)
    ret = _setproperty(109, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR substringData
  # retrieve substring of value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def substringData(arg0, arg1)
    ret = _invoke(111, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID appendData
  # append string to value
  #   BSTR arg0 --- data [IN]
  def appendData(arg0)
    ret = _invoke(112, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID insertData
  # insert string into value
  #   I4 arg0 --- offset [IN]
  #   BSTR arg1 --- data [IN]
  def insertData(arg0, arg1)
    ret = _invoke(113, [arg0, arg1], [VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID deleteData
  # delete string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def deleteData(arg0, arg1)
    ret = _invoke(114, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID replaceData
  # replace string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  #   BSTR arg2 --- data [IN]
  def replaceData(arg0, arg1, arg2)
    ret = _invoke(115, [arg0, arg1, arg2], [VT_I4, VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMCDATASection
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR data
  # value of the node
  def data()
    ret = _getproperty(109, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 length
  # number of characters in value
  def length()
    ret = _getproperty(110, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID data
  # value of the node
  def data=(arg0)
    ret = _setproperty(109, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR substringData
  # retrieve substring of value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def substringData(arg0, arg1)
    ret = _invoke(111, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID appendData
  # append string to value
  #   BSTR arg0 --- data [IN]
  def appendData(arg0)
    ret = _invoke(112, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID insertData
  # insert string into value
  #   I4 arg0 --- offset [IN]
  #   BSTR arg1 --- data [IN]
  def insertData(arg0, arg1)
    ret = _invoke(113, [arg0, arg1], [VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID deleteData
  # delete string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  def deleteData(arg0, arg1)
    ret = _invoke(114, [arg0, arg1], [VT_I4, VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID replaceData
  # replace string within the value
  #   I4 arg0 --- offset [IN]
  #   I4 arg1 --- count [IN]
  #   BSTR arg2 --- data [IN]
  def replaceData(arg0, arg1, arg2)
    ret = _invoke(115, [arg0, arg1, arg2], [VT_I4, VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMText splitText
  # split the text node into two text nodes at the position specified
  #   I4 arg0 --- offset [IN]
  def splitText(arg0)
    ret = _invoke(123, [arg0], [VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMProcessingInstruction
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR target
  # the target
  def target()
    ret = _getproperty(127, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR data
  # the data
  def data()
    ret = _getproperty(128, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID data
  # the data
  def data=(arg0)
    ret = _setproperty(128, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMEntityReference
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# structure for reporting parser errors
module IXMLDOMParseError
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # I4 errorCode
  # the error code
  def errorCode()
    ret = _getproperty(0, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR url
  # the URL of the XML document containing the error
  def url()
    ret = _getproperty(179, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR reason
  # the cause of the error
  def reason()
    ret = _getproperty(180, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR srcText
  # the data where the error occurred
  def srcText()
    ret = _getproperty(181, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 line
  # the line number in the XML document where the error occurred
  def line()
    ret = _getproperty(182, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 linepos
  # the character position in the line containing the error
  def linepos()
    ret = _getproperty(183, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 filepos
  # the absolute file position in the XML document containing the error
  def filepos()
    ret = _getproperty(184, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMNotation
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT publicId
  # the public ID
  def publicId()
    ret = _getproperty(136, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT systemId
  # the system ID
  def systemId()
    ret = _getproperty(137, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

#
module IXMLDOMEntity
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT publicId
  # the public ID
  def publicId()
    ret = _getproperty(140, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT systemId
  # the system ID
  def systemId()
    ret = _getproperty(141, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR notationName
  # the name of the notation
  def notationName()
    ret = _getproperty(142, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# XTL runtime object
module IXTLRuntime
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = _getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = _getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = _getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = _getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = _getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = _getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = _getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = _getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = _getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = _getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = _getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = _getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = _getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = _getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = _getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = _getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = _setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = _setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = _setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = _setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = _invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = _invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = _invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = _invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = _invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = _invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = _invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = _invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = _invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = _invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 uniqueID
  #   IXMLDOMNode arg0 --- pNode [IN]
  def uniqueID(arg0)
    ret = _invoke(187, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 depth
  #   IXMLDOMNode arg0 --- pNode [IN]
  def depth(arg0)
    ret = _invoke(188, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 childNumber
  #   IXMLDOMNode arg0 --- pNode [IN]
  def childNumber(arg0)
    ret = _invoke(189, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 ancestorChildNumber
  #   BSTR arg0 --- bstrNodeName [IN]
  #   IXMLDOMNode arg1 --- pNode [IN]
  def ancestorChildNumber(arg0, arg1)
    ret = _invoke(190, [arg0, arg1], [VT_BSTR, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 absoluteChildNumber
  #   IXMLDOMNode arg0 --- pNode [IN]
  def absoluteChildNumber(arg0)
    ret = _invoke(191, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR formatIndex
  #   I4 arg0 --- lIndex [IN]
  #   BSTR arg1 --- bstrFormat [IN]
  def formatIndex(arg0, arg1)
    ret = _invoke(192, [arg0, arg1], [VT_I4, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR formatNumber
  #   R8 arg0 --- dblNumber [IN]
  #   BSTR arg1 --- bstrFormat [IN]
  def formatNumber(arg0, arg1)
    ret = _invoke(193, [arg0, arg1], [VT_R8, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR formatDate
  #   VARIANT arg0 --- varDate [IN]
  #   BSTR arg1 --- bstrFormat [IN]
  #   VARIANT arg2 --- varDestLocale [IN]
  def formatDate(arg0, arg1, arg2=nil)
    ret = _invoke(194, [arg0, arg1, arg2], [VT_VARIANT, VT_BSTR, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR formatTime
  #   VARIANT arg0 --- varTime [IN]
  #   BSTR arg1 --- bstrFormat [IN]
  #   VARIANT arg2 --- varDestLocale [IN]
  def formatTime(arg0, arg1, arg2=nil)
    ret = _invoke(195, [arg0, arg1, arg2], [VT_VARIANT, VT_BSTR, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# W3C-DOM XML Document
class Microsoft_XMLDOM_1_0 # DOMDocument
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  attr_reader :dispatch
  attr_reader :clsid
  attr_reader :progid

  def initialize(obj = nil)
    @clsid = "{2933BF90-7B36-11D2-B20E-00C04F983E60}"
    @progid = "Microsoft.XMLDOM.1.0"
    if obj.nil?
      @dispatch = WIN32OLE.new(@progid)
    else
      @dispatch = obj
    end
  end

  def method_missing(cmd, *arg)
    @dispatch.method_missing(cmd, *arg)
  end

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = @dispatch._getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = @dispatch._getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = @dispatch._getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = @dispatch._getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = @dispatch._getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = @dispatch._getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = @dispatch._getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = @dispatch._getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = @dispatch._getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = @dispatch._getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = @dispatch._getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = @dispatch._getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = @dispatch._getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = @dispatch._getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = @dispatch._getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = @dispatch._getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = @dispatch._getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = @dispatch._getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = @dispatch._getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = @dispatch._getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = @dispatch._getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = @dispatch._getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocumentType doctype
  # node corresponding to the DOCTYPE
  def doctype()
    ret = @dispatch._getproperty(38, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMImplementation implementation
  # info on this DOM implementation
  def implementation()
    ret = @dispatch._getproperty(39, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMElement documentElement
  # the root of the tree
  def documentElement()
    ret = @dispatch._getproperty(40, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 readyState
  # get the state of the XML document
  def readyState()
    ret = @dispatch._getproperty(-525, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMParseError parseError
  # get the last parser error
  def parseError()
    ret = @dispatch._getproperty(59, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR url
  # get the URL for the loaded XML document
  def url()
    ret = @dispatch._getproperty(60, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL async
  # flag for asynchronous download
  def async()
    ret = @dispatch._getproperty(61, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL validateOnParse
  # indicates whether the parser performs validation
  def validateOnParse()
    ret = @dispatch._getproperty(65, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL resolveExternals
  # indicates whether the parser resolves references to external DTD/Entities/Schema
  def resolveExternals()
    ret = @dispatch._getproperty(66, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL preserveWhiteSpace
  # indicates whether the parser preserves whitespace
  def preserveWhiteSpace()
    ret = @dispatch._getproperty(67, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = @dispatch._setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = @dispatch._setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = @dispatch._setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = @dispatch._setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID async
  # flag for asynchronous download
  def async=(arg0)
    ret = @dispatch._setproperty(61, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID validateOnParse
  # indicates whether the parser performs validation
  def validateOnParse=(arg0)
    ret = @dispatch._setproperty(65, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID resolveExternals
  # indicates whether the parser resolves references to external DTD/Entities/Schema
  def resolveExternals=(arg0)
    ret = @dispatch._setproperty(66, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID preserveWhiteSpace
  # indicates whether the parser preserves whitespace
  def preserveWhiteSpace=(arg0)
    ret = @dispatch._setproperty(67, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID onreadystatechange
  # register a readystatechange event handler
  def onreadystatechange=(arg0)
    ret = @dispatch._setproperty(68, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID ondataavailable
  # register an ondataavailable event handler
  def ondataavailable=(arg0)
    ret = @dispatch._setproperty(69, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID ontransformnode
  # register an ontransformnode event handler
  def ontransformnode=(arg0)
    ret = @dispatch._setproperty(70, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = @dispatch._invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = @dispatch._invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = @dispatch._invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = @dispatch._invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = @dispatch._invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = @dispatch._invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = @dispatch._invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = @dispatch._invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = @dispatch._invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = @dispatch._invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMElement createElement
  # create an Element node
  #   BSTR arg0 --- tagName [IN]
  def createElement(arg0)
    ret = @dispatch._invoke(41, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocumentFragment createDocumentFragment
  # create a DocumentFragment node
  def createDocumentFragment()
    ret = @dispatch._invoke(42, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMText createTextNode
  # create a text node
  #   BSTR arg0 --- data [IN]
  def createTextNode(arg0)
    ret = @dispatch._invoke(43, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMComment createComment
  # create a comment node
  #   BSTR arg0 --- data [IN]
  def createComment(arg0)
    ret = @dispatch._invoke(44, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMCDATASection createCDATASection
  # create a CDATA section node
  #   BSTR arg0 --- data [IN]
  def createCDATASection(arg0)
    ret = @dispatch._invoke(45, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMProcessingInstruction createProcessingInstruction
  # create a processing instruction node
  #   BSTR arg0 --- target [IN]
  #   BSTR arg1 --- data [IN]
  def createProcessingInstruction(arg0, arg1)
    ret = @dispatch._invoke(46, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMAttribute createAttribute
  # create an attribute node
  #   BSTR arg0 --- name [IN]
  def createAttribute(arg0)
    ret = @dispatch._invoke(47, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMEntityReference createEntityReference
  # create an entity reference node
  #   BSTR arg0 --- name [IN]
  def createEntityReference(arg0)
    ret = @dispatch._invoke(49, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList getElementsByTagName
  # build a list of elements by name
  #   BSTR arg0 --- tagName [IN]
  def getElementsByTagName(arg0)
    ret = @dispatch._invoke(50, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode createNode
  # create a node of the specified node type and name
  #   VARIANT arg0 --- type [IN]
  #   BSTR arg1 --- name [IN]
  #   BSTR arg2 --- namespaceURI [IN]
  def createNode(arg0, arg1, arg2)
    ret = @dispatch._invoke(54, [arg0, arg1, arg2], [VT_VARIANT, VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nodeFromID
  # retrieve node from it's ID
  #   BSTR arg0 --- idString [IN]
  def nodeFromID(arg0)
    ret = @dispatch._invoke(56, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL load
  # load document from the specified XML source
  #   VARIANT arg0 --- xmlSource [IN]
  def load(arg0)
    ret = @dispatch._invoke(58, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID abort
  # abort an asynchronous download
  def abort()
    ret = @dispatch._invoke(62, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL loadXML
  # load the document from a string
  #   BSTR arg0 --- bstrXML [IN]
  def loadXML(arg0)
    ret = @dispatch._invoke(63, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID save
  # save the document to a specified desination
  #   VARIANT arg0 --- desination [IN]
  def save(arg0)
    ret = @dispatch._invoke(64, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT ondataavailable EVENT in XMLDOMDocumentEvents
  def ondataavailable()
    ret = @dispatch._invoke(198, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT onreadystatechange EVENT in XMLDOMDocumentEvents
  def onreadystatechange()
    ret = @dispatch._invoke(-609, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# W3C-DOM XML Document (Apartment)
class Microsoft_FreeThreadedXMLDOM_1_0 # DOMFreeThreadedDocument
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  attr_reader :dispatch
  attr_reader :clsid
  attr_reader :progid

  def initialize(obj = nil)
    @clsid = "{2933BF91-7B36-11D2-B20E-00C04F983E60}"
    @progid = "Microsoft.FreeThreadedXMLDOM.1.0"
    if obj.nil?
      @dispatch = WIN32OLE.new(@progid)
    else
      @dispatch = obj
    end
  end

  def method_missing(cmd, *arg)
    @dispatch.method_missing(cmd, *arg)
  end

  # BSTR nodeName
  # name of the node
  def nodeName()
    ret = @dispatch._getproperty(2, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeValue
  # value stored in the node
  def nodeValue()
    ret = @dispatch._getproperty(3, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DOMNodeType nodeType
  # the node's type
  def nodeType()
    ret = @dispatch._getproperty(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode parentNode
  # parent of the node
  def parentNode()
    ret = @dispatch._getproperty(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList childNodes
  # the collection of the node's children
  def childNodes()
    ret = @dispatch._getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode firstChild
  # first child of the node
  def firstChild()
    ret = @dispatch._getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode lastChild
  # first child of the node
  def lastChild()
    ret = @dispatch._getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode previousSibling
  # left sibling of the node
  def previousSibling()
    ret = @dispatch._getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nextSibling
  # right sibling of the node
  def nextSibling()
    ret = @dispatch._getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNamedNodeMap attributes
  # the collection of the node's attributes
  def attributes()
    ret = @dispatch._getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocument ownerDocument
  # document that contains the node
  def ownerDocument()
    ret = @dispatch._getproperty(18, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR nodeTypeString
  # the type of node in string form
  def nodeTypeString()
    ret = @dispatch._getproperty(21, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR text
  # text content of the node and subtree
  def text()
    ret = @dispatch._getproperty(24, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL specified
  # indicates whether node is a default value
  def specified()
    ret = @dispatch._getproperty(22, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode definition
  # pointer to the definition of the node in the DTD or schema
  def definition()
    ret = @dispatch._getproperty(23, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue()
    ret = @dispatch._getproperty(25, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT dataType
  # the data type of the node
  def dataType()
    ret = @dispatch._getproperty(26, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR xml
  # return the XML source for the node and each of its descendants
  def xml()
    ret = @dispatch._getproperty(27, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL parsed
  # has sub-tree been completely parsed
  def parsed()
    ret = @dispatch._getproperty(31, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR namespaceURI
  # the URI for the namespace applying to the node
  def namespaceURI()
    ret = @dispatch._getproperty(32, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR prefix
  # the prefix for the namespace applying to the node
  def prefix()
    ret = @dispatch._getproperty(33, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR baseName
  # the base name of the node (nodename with the prefix stripped off)
  def baseName()
    ret = @dispatch._getproperty(34, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocumentType doctype
  # node corresponding to the DOCTYPE
  def doctype()
    ret = @dispatch._getproperty(38, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMImplementation implementation
  # info on this DOM implementation
  def implementation()
    ret = @dispatch._getproperty(39, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMElement documentElement
  # the root of the tree
  def documentElement()
    ret = @dispatch._getproperty(40, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 readyState
  # get the state of the XML document
  def readyState()
    ret = @dispatch._getproperty(-525, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMParseError parseError
  # get the last parser error
  def parseError()
    ret = @dispatch._getproperty(59, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR url
  # get the URL for the loaded XML document
  def url()
    ret = @dispatch._getproperty(60, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL async
  # flag for asynchronous download
  def async()
    ret = @dispatch._getproperty(61, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL validateOnParse
  # indicates whether the parser performs validation
  def validateOnParse()
    ret = @dispatch._getproperty(65, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL resolveExternals
  # indicates whether the parser resolves references to external DTD/Entities/Schema
  def resolveExternals()
    ret = @dispatch._getproperty(66, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL preserveWhiteSpace
  # indicates whether the parser preserves whitespace
  def preserveWhiteSpace()
    ret = @dispatch._getproperty(67, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeValue
  # value stored in the node
  def nodeValue=(arg0)
    ret = @dispatch._setproperty(3, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID text
  # text content of the node and subtree
  def text=(arg0)
    ret = @dispatch._setproperty(24, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID nodeTypedValue
  # get the strongly typed value of the node
  def nodeTypedValue=(arg0)
    ret = @dispatch._setproperty(25, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID dataType
  # the data type of the node
  def dataType=(arg0)
    ret = @dispatch._setproperty(26, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID async
  # flag for asynchronous download
  def async=(arg0)
    ret = @dispatch._setproperty(61, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID validateOnParse
  # indicates whether the parser performs validation
  def validateOnParse=(arg0)
    ret = @dispatch._setproperty(65, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID resolveExternals
  # indicates whether the parser resolves references to external DTD/Entities/Schema
  def resolveExternals=(arg0)
    ret = @dispatch._setproperty(66, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID preserveWhiteSpace
  # indicates whether the parser preserves whitespace
  def preserveWhiteSpace=(arg0)
    ret = @dispatch._setproperty(67, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID onreadystatechange
  # register a readystatechange event handler
  def onreadystatechange=(arg0)
    ret = @dispatch._setproperty(68, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID ondataavailable
  # register an ondataavailable event handler
  def ondataavailable=(arg0)
    ret = @dispatch._setproperty(69, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID ontransformnode
  # register an ontransformnode event handler
  def ontransformnode=(arg0)
    ret = @dispatch._setproperty(70, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode insertBefore
  # insert a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   VARIANT arg1 --- refChild [IN]
  def insertBefore(arg0, arg1)
    ret = @dispatch._invoke(13, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode replaceChild
  # replace a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  #   IXMLDOMNode arg1 --- oldChild [IN]
  def replaceChild(arg0, arg1)
    ret = @dispatch._invoke(14, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode removeChild
  # remove a child node
  #   IXMLDOMNode arg0 --- childNode [IN]
  def removeChild(arg0)
    ret = @dispatch._invoke(15, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode appendChild
  # append a child node
  #   IXMLDOMNode arg0 --- newChild [IN]
  def appendChild(arg0)
    ret = @dispatch._invoke(16, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL hasChildNodes
  def hasChildNodes()
    ret = @dispatch._invoke(17, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode cloneNode
  #   BOOL arg0 --- deep [IN]
  def cloneNode(arg0)
    ret = @dispatch._invoke(19, [arg0], [VT_BOOL])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR transformNode
  # apply the stylesheet to the subtree
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  def transformNode(arg0)
    ret = @dispatch._invoke(28, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList selectNodes
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectNodes(arg0)
    ret = @dispatch._invoke(29, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode selectSingleNode
  # execute query on the subtree
  #   BSTR arg0 --- queryString [IN]
  def selectSingleNode(arg0)
    ret = @dispatch._invoke(30, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID transformNodeToObject
  # apply the stylesheet to the subtree, returning the result through a document or a stream
  #   IXMLDOMNode arg0 --- stylesheet [IN]
  #   VARIANT arg1 --- outputObject [IN]
  def transformNodeToObject(arg0, arg1)
    ret = @dispatch._invoke(35, [arg0, arg1], [VT_BYREF|VT_DISPATCH, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMElement createElement
  # create an Element node
  #   BSTR arg0 --- tagName [IN]
  def createElement(arg0)
    ret = @dispatch._invoke(41, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMDocumentFragment createDocumentFragment
  # create a DocumentFragment node
  def createDocumentFragment()
    ret = @dispatch._invoke(42, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMText createTextNode
  # create a text node
  #   BSTR arg0 --- data [IN]
  def createTextNode(arg0)
    ret = @dispatch._invoke(43, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMComment createComment
  # create a comment node
  #   BSTR arg0 --- data [IN]
  def createComment(arg0)
    ret = @dispatch._invoke(44, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMCDATASection createCDATASection
  # create a CDATA section node
  #   BSTR arg0 --- data [IN]
  def createCDATASection(arg0)
    ret = @dispatch._invoke(45, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMProcessingInstruction createProcessingInstruction
  # create a processing instruction node
  #   BSTR arg0 --- target [IN]
  #   BSTR arg1 --- data [IN]
  def createProcessingInstruction(arg0, arg1)
    ret = @dispatch._invoke(46, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMAttribute createAttribute
  # create an attribute node
  #   BSTR arg0 --- name [IN]
  def createAttribute(arg0)
    ret = @dispatch._invoke(47, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMEntityReference createEntityReference
  # create an entity reference node
  #   BSTR arg0 --- name [IN]
  def createEntityReference(arg0)
    ret = @dispatch._invoke(49, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNodeList getElementsByTagName
  # build a list of elements by name
  #   BSTR arg0 --- tagName [IN]
  def getElementsByTagName(arg0)
    ret = @dispatch._invoke(50, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode createNode
  # create a node of the specified node type and name
  #   VARIANT arg0 --- type [IN]
  #   BSTR arg1 --- name [IN]
  #   BSTR arg2 --- namespaceURI [IN]
  def createNode(arg0, arg1, arg2)
    ret = @dispatch._invoke(54, [arg0, arg1, arg2], [VT_VARIANT, VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # IXMLDOMNode nodeFromID
  # retrieve node from it's ID
  #   BSTR arg0 --- idString [IN]
  def nodeFromID(arg0)
    ret = @dispatch._invoke(56, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL load
  # load document from the specified XML source
  #   VARIANT arg0 --- xmlSource [IN]
  def load(arg0)
    ret = @dispatch._invoke(58, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID abort
  # abort an asynchronous download
  def abort()
    ret = @dispatch._invoke(62, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BOOL loadXML
  # load the document from a string
  #   BSTR arg0 --- bstrXML [IN]
  def loadXML(arg0)
    ret = @dispatch._invoke(63, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID save
  # save the document to a specified desination
  #   VARIANT arg0 --- desination [IN]
  def save(arg0)
    ret = @dispatch._invoke(64, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT ondataavailable EVENT in XMLDOMDocumentEvents
  def ondataavailable()
    ret = @dispatch._invoke(198, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT onreadystatechange EVENT in XMLDOMDocumentEvents
  def onreadystatechange()
    ret = @dispatch._invoke(-609, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# IXMLHttpRequest Interface
module IXMLHttpRequest
  include WIN32OLE::VARIANT
  attr_reader :lastargs

  # I4 status
  # Get HTTP status code
  def status()
    ret = _getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR statusText
  # Get HTTP status text
  def statusText()
    ret = _getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DISPATCH responseXML
  # Get response body
  def responseXML()
    ret = _getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR responseText
  # Get response body
  def responseText()
    ret = _getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT responseBody
  # Get response body
  def responseBody()
    ret = _getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT responseStream
  # Get response body
  def responseStream()
    ret = _getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 readyState
  # Get ready state
  def readyState()
    ret = _getproperty(13, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID onreadystatechange
  # Register a complete event handler
  def onreadystatechange=(arg0)
    ret = _setproperty(14, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID open
  # Open HTTP connection
  #   BSTR arg0 --- bstrMethod [IN]
  #   BSTR arg1 --- bstrUrl [IN]
  #   VARIANT arg2 --- varAsync [IN]
  #   VARIANT arg3 --- bstrUser [IN]
  #   VARIANT arg4 --- bstrPassword [IN]
  def open(arg0, arg1, arg2=nil, arg3=nil, arg4=nil)
    ret = _invoke(1, [arg0, arg1, arg2, arg3, arg4], [VT_BSTR, VT_BSTR, VT_VARIANT, VT_VARIANT, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID setRequestHeader
  # Add HTTP request header
  #   BSTR arg0 --- bstrHeader [IN]
  #   BSTR arg1 --- bstrValue [IN]
  def setRequestHeader(arg0, arg1)
    ret = _invoke(2, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR getResponseHeader
  # Get HTTP response header
  #   BSTR arg0 --- bstrHeader [IN]
  def getResponseHeader(arg0)
    ret = _invoke(3, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR getAllResponseHeaders
  # Get all HTTP response headers
  def getAllResponseHeaders()
    ret = _invoke(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID send
  # Send HTTP request
  #   VARIANT arg0 --- varBody [IN]
  def send(arg0=nil)
    ret = _invoke(5, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID abort
  # Abort HTTP request
  def abort()
    ret = _invoke(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# XML HTTP Request class.
class Microsoft_XMLHTTP_1 # XMLHTTPRequest
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  attr_reader :dispatch
  attr_reader :clsid
  attr_reader :progid

  def initialize(obj = nil)
    @clsid = "{ED8C108E-4349-11D2-91A4-00C04F7969E8}"
    @progid = "Microsoft.XMLHTTP.1"
    if obj.nil?
      @dispatch = WIN32OLE.new(@progid)
    else
      @dispatch = obj
    end
  end

  def method_missing(cmd, *arg)
    @dispatch.method_missing(cmd, *arg)
  end

  # I4 status
  # Get HTTP status code
  def status()
    ret = @dispatch._getproperty(7, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR statusText
  # Get HTTP status text
  def statusText()
    ret = @dispatch._getproperty(8, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # DISPATCH responseXML
  # Get response body
  def responseXML()
    ret = @dispatch._getproperty(9, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR responseText
  # Get response body
  def responseText()
    ret = @dispatch._getproperty(10, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT responseBody
  # Get response body
  def responseBody()
    ret = @dispatch._getproperty(11, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VARIANT responseStream
  # Get response body
  def responseStream()
    ret = @dispatch._getproperty(12, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 readyState
  # Get ready state
  def readyState()
    ret = @dispatch._getproperty(13, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID onreadystatechange
  # Register a complete event handler
  def onreadystatechange=(arg0)
    ret = @dispatch._setproperty(14, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID open
  # Open HTTP connection
  #   BSTR arg0 --- bstrMethod [IN]
  #   BSTR arg1 --- bstrUrl [IN]
  #   VARIANT arg2 --- varAsync [IN]
  #   VARIANT arg3 --- bstrUser [IN]
  #   VARIANT arg4 --- bstrPassword [IN]
  def open(arg0, arg1, arg2=nil, arg3=nil, arg4=nil)
    ret = @dispatch._invoke(1, [arg0, arg1, arg2, arg3, arg4], [VT_BSTR, VT_BSTR, VT_VARIANT, VT_VARIANT, VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID setRequestHeader
  # Add HTTP request header
  #   BSTR arg0 --- bstrHeader [IN]
  #   BSTR arg1 --- bstrValue [IN]
  def setRequestHeader(arg0, arg1)
    ret = @dispatch._invoke(2, [arg0, arg1], [VT_BSTR, VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR getResponseHeader
  # Get HTTP response header
  #   BSTR arg0 --- bstrHeader [IN]
  def getResponseHeader(arg0)
    ret = @dispatch._invoke(3, [arg0], [VT_BSTR])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # BSTR getAllResponseHeaders
  # Get all HTTP response headers
  def getAllResponseHeaders()
    ret = @dispatch._invoke(4, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID send
  # Send HTTP request
  #   VARIANT arg0 --- varBody [IN]
  def send(arg0=nil)
    ret = @dispatch._invoke(5, [arg0], [VT_VARIANT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID abort
  # Abort HTTP request
  def abort()
    ret = @dispatch._invoke(6, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# XML Data Source Object
class Microsoft_XMLDSO_1_0 # XMLDSOControl
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  attr_reader :dispatch
  attr_reader :clsid
  attr_reader :progid

  def initialize(obj = nil)
    @clsid = "{550DDA30-0541-11D2-9CA9-0060B0EC3D39}"
    @progid = "Microsoft.XMLDSO.1.0"
    if obj.nil?
      @dispatch = WIN32OLE.new(@progid)
    else
      @dispatch = obj
    end
  end

  def method_missing(cmd, *arg)
    @dispatch.method_missing(cmd, *arg)
  end

  # IXMLDOMDocument XMLDocument
  def XMLDocument()
    ret = @dispatch._getproperty(65537, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 JavaDSOCompatible
  def JavaDSOCompatible()
    ret = @dispatch._getproperty(65538, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # I4 readyState
  def readyState()
    ret = @dispatch._getproperty(-525, [], [])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID XMLDocument
  def XMLDocument=(arg0)
    ret = @dispatch._setproperty(65537, [arg0], [VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # VOID JavaDSOCompatible
  def JavaDSOCompatible=(arg0)
    ret = @dispatch._setproperty(65538, [arg0], [VT_I4])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end

# Constants that define types for IXMLElement.
module OLEtagXMLEMEM_TYPE
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  XMLELEMTYPE_ELEMENT = 0
  XMLELEMTYPE_TEXT = 1
  XMLELEMTYPE_COMMENT = 2
  XMLELEMTYPE_DOCUMENT = 3
  XMLELEMTYPE_DTD = 4
  XMLELEMTYPE_PI = 5
  XMLELEMTYPE_OTHER = 6
end

# XMLDocument extends IXML Document.  It is obsolete.  You should use DOMDocument.  This object should not be confused with the XMLDocument property on the XML data island.
class Msxml # XMLDocument
  include WIN32OLE::VARIANT
  attr_reader :lastargs
  attr_reader :dispatch
  attr_reader :clsid
  attr_reader :progid

  def initialize(obj = nil)
    @clsid = "{CFC399AF-D876-11D0-9C10-00C04FC99C8E}"
    @progid = "Msxml"
    if obj.nil?
      @dispatch = WIN32OLE.new(@progid)
    else
      @dispatch = obj
    end
  end

  def method_missing(cmd, *arg)
    @dispatch.method_missing(cmd, *arg)
  end

  # HRESULT url
  # set URL to load an XML document from the URL.
  #   BSTR arg0 --- p [IN]
  def url=(arg0)
    ret = @dispatch._setproperty(65641, [arg0], [VT_BSTR, VT_HRESULT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT charset
  # get encoding.
  #   BSTR arg0 --- p [IN]
  def charset=(arg0)
    ret = @dispatch._setproperty(65645, [arg0], [VT_BSTR, VT_HRESULT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT async
  # get asynchronous loading flag.
  #   BOOL arg0 --- pf [IN]
  def async=(arg0)
    ret = @dispatch._setproperty(65649, [arg0], [VT_BOOL, VT_HRESULT])
    @lastargs = WIN32OLE::ARGV
    ret
  end

  # HRESULT root
  # get root IXMLElement of the XML document.
  #   IXMLElement2,IXMLElement2 arg0 --- p [OUT]
  def root
    OLEProperty.new(@dispatch, 65637, [VT_BYREF|VT_BYREF|VT_DISPATCH], [VT_BYREF|VT_BYREF|VT_DISPATCH, VT_HRESULT])
  end

  # HRESULT url
  # set URL to load an XML document from the URL.
  #   BSTR arg0 --- p [OUT]
  def url
    OLEProperty.new(@dispatch, 65641, [VT_BYREF|VT_BSTR], [VT_BYREF|VT_BSTR, VT_HRESULT])
  end

  # HRESULT readyState
  # get ready state.
  #   I4 arg0 --- pl [OUT]
  def readyState
    OLEProperty.new(@dispatch, 65643, [VT_BYREF|VT_I4], [VT_BYREF|VT_I4, VT_HRESULT])
  end

  # HRESULT charset
  # get encoding.
  #   BSTR arg0 --- p [OUT]
  def charset
    OLEProperty.new(@dispatch, 65645, [VT_BYREF|VT_BSTR], [VT_BYREF|VT_BSTR, VT_HRESULT])
  end

  # HRESULT version
  # get XML version number.
  #   BSTR arg0 --- p [OUT]
  def version
    OLEProperty.new(@dispatch, 65646, [VT_BYREF|VT_BSTR], [VT_BYREF|VT_BSTR, VT_HRESULT])
  end

  # HRESULT doctype
  # get document type.
  #   BSTR arg0 --- p [OUT]
  def doctype
    OLEProperty.new(@dispatch, 65647, [VT_BYREF|VT_BSTR], [VT_BYREF|VT_BSTR, VT_HRESULT])
  end

  # HRESULT async
  # get asynchronous loading flag.
  #   BOOL arg0 --- pf [OUT]
  def async
    OLEProperty.new(@dispatch, 65649, [VT_BYREF|VT_BOOL], [VT_BYREF|VT_BOOL, VT_HRESULT])
  end

  # HRESULT createElement
  # create different types of IXMLElements.
  #   VARIANT arg0 --- vType [IN]
  #   VARIANT arg1 --- var1 [IN]
  #   IXMLElement2,IXMLElement2 arg2 --- ppElem [OUT]
  def createElement(arg0, arg1=nil, arg2=nil)
    ret = @dispatch._invoke(65644, [arg0, arg1, arg2], [VT_VARIANT, VT_VARIANT, VT_BYREF|VT_BYREF|VT_DISPATCH])
    @lastargs = WIN32OLE::ARGV
    ret
  end
end
