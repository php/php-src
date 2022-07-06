--TEST--
Bug #41148 (pcre 7.0 regression)
--FILE--
<?php

    $letexte="<br><br>";
    $ligne_horizontale = $puce = $debut_intertitre = $fin_intertitre = '';

    $cherche1 = array(
        /* 0 */     "/\n(----+|____+)/S",
        /* 1 */     "/\n-- */S",
        /* 2 */     "/\n- */S",
        /* 3 */     "/\n_ +/S",
        /* 4 */   "/(^|[^{])[{][{][{]/S",
        /* 5 */   "/[}][}][}]($|[^}])/S",
        /* 6 */     "/(( *)\n){2,}(<br[[:space:]]*\/?".">)?/S",
        /* 7 */     "/[{][{]/S",
        /* 8 */     "/[}][}]/S",
        /* 9 */     "/[{]/S",
        /* 10 */    "/[}]/S",
        /* 11 */    "/(<br[[:space:]]*\/?".">){2,}/S",
        /* 12 */    "/<p>([\n]*(<br[[:space:]]*\/?".">)*)*/S",
        /* 13 */    "/<quote>/S",
        /* 14 */    "/<\/quote>/S"
    );
    $remplace1 = array(
        /* 0 */     "\n\n$ligne_horizontale\n\n",
        /* 1 */     "\n<br />&mdash;&nbsp;",
        /* 2 */     "\n<br />$puce&nbsp;",
        /* 3 */     "\n<br />",
        /* 4 */     "\$1\n\n$debut_intertitre",
        /* 5 */     "$fin_intertitre\n\n\$1",
        /* 6 */     "<p>",
        /* 7 */     "<strong class=\"spip\">",
        /* 8 */     "</strong>",
        /* 9 */     "<i class=\"spip\">",
        /* 10 */    "</i>",
        /* 11 */    "<p>",
        /* 12 */    "<p>",
        /* 13 */    "<blockquote class=\"spip\"><p>",
        /* 14 */    "</blockquote><p>"
    );
    $letexte = preg_replace($cherche1, $remplace1, $letexte);
    $letexte = preg_replace("@^ <br />@S", "", $letexte);

    print $letexte;

?>
--EXPECT--
<p>
