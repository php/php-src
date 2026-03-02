--TEST--
Bug #24635 (crash on dtor calling other functions)
--FILE--
<?php
class SiteClass {
    public $page;
    function __construct()	{ $this->page = new PageClass(); }
}
class PageClass {
    function Display() {
        $section = new SectionClass("PageClass::Display");
    }
}
class SectionClass {
    public $Comment;
    function __construct($comment) {
        $this->Comment = $comment;
    }
    function __destruct() {
        out($this->Comment); // this line doesn't crash PHP
        out("\n<!-- End Section: " . $this->Comment . "-->"); // this line
    }
}
function out($code) { return; }
$site = new SiteClass();
$site->page->Display();
echo "OK\n";
?>
--EXPECT--
OK
