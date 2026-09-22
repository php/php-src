--TEST--
GH-15101: _ir_RSTORE: Assertion `ctx->control' failed
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
--FILE--
<?php
class A {
   function test($context, $token) { 
       if ($token instanceof DoctypeToken) {
            $this->processDoctypeToken($context, $token);
        }
    }
    private function processDoctypeToken(TreeBuilderContext $context, DoctypeToken $token): void
    {
        $publicId = $token->publicIdentifier;
        $systemId = $token->systemIdentifier;
        $name = $token->name;

        if ($name !== 'html'
            || $publicId !== null
            || ($systemId !== null && $systemId !== 'about:legacy-compat')) {
        }

        $doctype = new DocumentType($context->document, $name ?? '', $publicId ?? '', $systemId ?? '');
    }
}
class Document {
}
final class TreeBuilderContext {
    public $document;
    public function __construct() {
        $this->document = new Document;
    }
}
abstract class Node {
    public const DOCUMENT_TYPE_NODE = 10;

    protected function __construct(Document $document, int $nodeType)
    {
    }
}
class DocumentType extends Node {
    public readonly string $name;
    public readonly string $publicId;
    public readonly string $systemId;

    public function __construct(
        Document $document,
        string $name,
        string $publicId = '',
        string $systemId = '') {
        parent::__construct($document, self::DOCUMENT_TYPE_NODE);
    }
}
class DoctypeToken {
    public $publicIdentifier;
    public $name;
    public $systemIdentifier;
}

$a = new A;
$doc = new TreeBuilderContext();
$t = new DoctypeToken();
$t->name = "html";
foreach ([$doc, $t] as $token) {
	$a->test($doc, $token);
}
?>
DONE
--EXPECT--
DONE
