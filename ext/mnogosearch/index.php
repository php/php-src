<?

/* 
    $Source$
    $Id$ 
*/

/*   mnoGoSearch-php-lite v.1.3
 *   for mnoGoSearch ( formely known as UdmSearch ) free web search engine
 *   (C) 2001-2002 by Sergey Kartashoff <gluke@mail.ru>,
 *               mnoGoSearch Developers Team <devel@mnogosearch.org>
 */

if (!extension_loaded('mnogosearch')) {
	print "<b>This script requires PHP4.0.5+ with mnoGoSearch extension</b>";
	exit;
}

// maximal page number to view
$MAX_NP=1000;

// maximum results per page
$MAX_PS=100;

// (optional) Enable autowild feature for url limits
// 'yes' - default value, can be 'yes' or 'no'
// $auto_wild='yes';

/* variables section */

if (Udm_Api_Version() >= 30204) {
	$dbaddr='mysql://mnogo:mnogo@/mnogo/?dbmode=single';
} else {
	$dbaddr='mysql://mnogo:mnogo@/mnogo/';
	$dbmode='single';
}

$localcharset='koi8-r';
$browsercharset='utf-8';
$phrase=$cache=$crosswords='no';
$ispelluseprefixes=$trackquery='no';
$spell_host=$vardir=$datadir='';
$ispellmode='text';

$affix_file=array();
$spell_file=array();
$stopwordtable_arr=array();
$stopwordfile_arr=array();
$synonym_arr=array();
$searchd_arr=array();

// $affix_file['en']='/opt/udm/ispell/en.aff';
// $affix_file['ru']='/opt/udm/ispell/ru.aff';
// $spell_file['en']='/opt/udm/ispell/en.dict';
// $spell_file['ru']='/opt/udm/ispell/ru.dict';
 $stopwordtable_arr[]='stopword';
// $stopwordfile_arr[]='stopwords.txt';
// $synonym_arr[]='/opt/udm/synonym/english.syn';
$searchd_arr[]='localhost';

$minwordlength=1;
$maxwordlength=32;

$storedocurl="/cgi-bin/storedoc.cgi";

/* initialisation section */

if(isset($HTTP_GET_VARS)){
  while(list($var, $val)=each($HTTP_GET_VARS)){
    $$var=$val;
  }
}
if(isset($HTTP_POST_VARS)){
  while(list($var, $val)=each($HTTP_POST_VARS)){
    $$var=$val;
  }
}
if(isset($HTTP_COOKIE_VARS)){
  while(list($var, $val)=each($HTTP_COOKIE_VARS)){
    $$var=$val;
  }
}
if(isset($HTTP_SERVER_VARS)){
  while(list($var, $val)=each($HTTP_SERVER_VARS)){
    $$var=$val;
  }
}

$self=$PHP_SELF;

if ($ps=="") $ps=20;
if ($np=="") $np=0;
if ($o=="") $o=0;

if (($dt!='back') && ($dt!='er') && ($dt!='range')) $dt='back';
if ($dp=="") $dp=0;
if (($dx!=0) && ($dx!=-1) && ($dx!=1)) $dx=0;
if ($dy<1970) $dy=1970;
if (($dm<0) || ($dm>11)) $dm=0;
if (($dd<=0) || ($dd>31)) $dd="01";

$db=urldecode($db);
$de=urldecode($de);

if ($db=="") $db='01/01/1970';
if ($de=="") $de='31/12/2020';

$storedaddr="localhost";
$storedocurl='/cgi-bin/storedoc.cgi';

if (isset($q)) {
	$q=urldecode($q);
        $have_query_flag=1;
} else {
	$have_query_flag=0;
}

$ul=urldecode($ul);
$tag=urldecode($t); 
$lang=urldecode($lang); 

$query_orig=$q;

if (isset($CHARSET_SAVED_QUERY_STRING)) {
	$q_local=urldecode($CHARSET_SAVED_QUERY_STRING);
	if (preg_match('/q=([^&]*)\&/',$q_local,$param)) {
		$q_local=urlencode($param[1]);
	} elseif (preg_match('/q=(.*)$/',$q_local,$param)) {
		$q_local=urlencode($param[1]);
	} else {
		$q_local=urlencode($q);
	}
} else {
	$q_local=urlencode($q);
}

$ul_local=urlencode($ul);
$t_local=urlencode($tag);
$db_local=urlencode($db);
$de_local=urlencode($de);
$lang_local=urlencode($lang);

if (($MAX_NP > 0) && ($np>$MAX_NP)) $np=$MAX_NP;
if (($MAX_PS > 0) && ($ps>$MAX_PS)) $ps=$MAX_PS;

// -----------------------------------------------
// print_bottom()
// -----------------------------------------------
function print_bottom(){
	print ("<HR><center><img src=\"http://mnogosearch.org/img/mnogo.gif\">\n");
	print ("<font size=\"-1\">Powered by <a href=\"http://mnogosearch.org/\">mnoGoSearch</a></font><br>\n");
	print ("<p></BODY></HTML>\n");
}


// -----------------------------------------------
// print_error_local($str)
// -----------------------------------------------
function print_error_local($str){
    print ("<CENTER><FONT COLOR=\"#FF0000\">An error occured!</FONT>\n");
    print ("<P><B>$str</B></CENTER>\n");
    print_bottom();    
    exit;
}

// -----------------------------------------------
// exit_local()
// -----------------------------------------------
function exit_local($print_err = 1) {
    drop_temp_table($print_err);
    exit;
}

// -----------------------------------------------
// format_lastmod($lastmod)
// -----------------------------------------------
function format_lastmod($lastmod) {
	$temp=$lastmod;
	if (!$temp) $temp = 'undefined';
	else $temp = strftime('%a, %d %b %Y %H:%M:%S %Z',$temp);

	return $temp;
}

// -----------------------------------------------
// format_dp($dp)
// -----------------------------------------------
function format_dp($dp) {
	$result=0;

	while ($dp != '') {		
		if (preg_match('/^([\-\+]?\d+)([sMhdmy]?)/',$dp,$param)) {			
			switch ($param[2]) {
				case 's':  $multiplier=1; break;
                                case 'M':  $multiplier=60; break;
                                case 'h':  $multiplier=3600; break;
                                case 'd':  $multiplier=3600*24; break;
                                case 'm':  $multiplier=3600*24*31; break;
                                case 'y':  $multiplier=3600*24*365; break;
				default: $multiplier=1;
			}

			$result += $param[1]*$multiplier;
                        $dp=preg_replace("/^[\-\+]?\d+$param[2]/",'',$dp);
		} else {
			return 0;
		}
	}

	return $result;
}

// -----------------------------------------------
// format_userdate($date)
// -----------------------------------------------
function format_userdate($date) {
	$result=0;

	if (preg_match('/^(\d+)\/(\d+)\/(\d+)$/',$date,$param)) {
		$day=$param[1];
		$mon=$param[2];
		$year=$param[3];

		$result=mktime(0,0,0,$mon,$day,$year);
	}

	return $result;
}

// -----------------------------------------------
// ParseDocText($text)
// -----------------------------------------------
function ParseDocText($text){
    global $all_words;
    global $hlbeg, $hlend;
       
    $str=$text;
    
    if (Udm_Api_Version() < 30200) {
    	for ($i=0; $i<count($all_words); $i++) {
		$word=$all_words[$i];
		$str = preg_replace("/([\s\t\r\n\~\!\@\#\$\%\^\&\*\(\)\-\_\=\+\\\|\{\}\[\]\;\:\'\"\<\>\?\/\,\.]+)($word)/i","\\1$hlbeg\\2$hlend",$str);
		$str = preg_replace("/^($word)/i","$hlbeg\\1$hlend",$str);
    	}
    } else {
    	$str = str_replace("\2",$hlbeg,$str);
    	$str = str_replace("\3",$hlend,$str);
    }

    return $str;
}

// -----------------------------------------------
// print_template($t,$echo=1)
// -----------------------------------------------
function print_template($t,$echo=1){
    global $templates, $udm_agent;
    global $first_doc, $last_doc, $found, $query_orig, $error, $self;
    global $nav, $wordinfo;
    global $url, $ue, $o, $cat;
    global $clones, $searchtime;
    global $title, $rating, $desc, $contype, $lastmod, $docsize, $ndoc;
    global $keyw, $text, $category;
    global $crc, $Randoms, $rec_id, $DEBUG;
    global $lang_url_translation, $phpver;

 
    $str=$templates["$t"][$o];
    if ($str == '') $str=$templates["$t"][0];

    $str=ereg_replace('\$f', "$first_doc", $str);
    $str=ereg_replace('\$l', "$last_doc", $str);
    $str=ereg_replace('\$t', "$found", $str);
    $str=ereg_replace('\$A', $self, $str);

    $str=ereg_replace('\$Q', HtmlSpecialChars(StripSlashes($query_orig)), $str);
    $str=ereg_replace('\$q', urlencode($query_orig), $str);
    $str=eregi_replace('\$UE', $ue, $str);

    $str=ereg_replace('\$E', $error, $str);
    $str=ereg_replace('\$W', $wordinfo, $str);

    $str=ereg_replace('\$V', $nav, $str);

    if ($lang_url_translation == 'yes') {
    	$nolangurl = ereg_replace("\.[a-z]{2}\.[a-z]{2,4}$", "", $url);
        $str=ereg_replace('\$DU', $nolangurl, $str);
    } else {
    	$str=ereg_replace('\$DU', $url, $str);
    }

    $str=ereg_replace('\$DT', $title, $str);
    $str=ereg_replace('\$DR', "$rating", $str);
    $str=ereg_replace('\$DX', $text, $str);
    $str=ereg_replace('\$DE', ($desc != '')?$desc:$text, $str);
    $str=ereg_replace('\$DC', $contype, $str);
    $str=ereg_replace('\$DM', $lastmod, $str);
    $str=ereg_replace('\$DS', "$docsize", $str);
    $str=ereg_replace('\$DN', "$ndoc", $str);
    $str=ereg_replace('\$DD', $desc, $str);
    $str=ereg_replace('\$DK', $keyw, $str);
    $str=ereg_replace('\$SearchTime', "$searchtime", $str);

    if ($phpver >= 40006) {
        if (ereg('\$CP',$str)) {
        	if ($temp_cp_arr=Udm_Cat_Path($udm_agent,$cat)) {
	        	reset($temp_cp_arr);
	        	$temp_cp='';
	        	for ($i=0; $i<count($temp_cp_arr); $i+=2) {
	        		$cp_path=$temp_cp_arr[$i];
	        		$cp_name=$temp_cp_arr[$i+1];
	        		$temp_cp .= " &gt; <a href=\"$PHP_SELF?cat=$cp_path\">$cp_name</a> ";
	        	}
	        	$str=ereg_replace('\$CP', $temp_cp, $str);
		} else $str=ereg_replace('\$CP', '', $str);
        }    	

        if (ereg('\$CS',$str)) {
        	if ($temp_cp_arr=Udm_Cat_List($udm_agent,$cat)) {
	        	reset($temp_cp_arr);
	        	$temp_cp='';
	                for ($i=0; $i<count($temp_cp_arr); $i+=2) {
	        		$cp_path=$temp_cp_arr[$i];
	        		$cp_name=$temp_cp_arr[$i+1];
	        		$temp_cp .= "<a href=\"$PHP_SELF?cat=$cp_path\">$cp_name</a><br>";
	        	}
	                $str=ereg_replace('\$CS', $temp_cp, $str);
		} else $str=ereg_replace('\$CS', '', $str);
        }    	

        if (ereg('\$DY',$str)) {
        	if ($temp_cp_arr=Udm_Cat_Path($udm_agent,$category)) {
	        	reset($temp_cp_arr);
	        	$temp_cp='';
	                for ($i=0; $i<count($temp_cp_arr); $i+=2) {
	        		$cp_path=$temp_cp_arr[$i];
	        		$cp_name=$temp_cp_arr[$i+1];
	        		$temp_cp .= " &gt; <a href=\"$PHP_SELF?cat=$cp_path\">$cp_name</a> ";
	        	}
	                $str=ereg_replace('\$DY', $temp_cp, $str);
		} else $str=ereg_replace('\$DY', '', $str);
        }
    } else {
    	$str=ereg_replace('\$CP', '', $str);
        $str=ereg_replace('\$CS', '', $str);
        $str=ereg_replace('\$DY', '', $str);
    }

    $sub=$str;
    while($sub=strstr($sub,'$r')){
	$sub=substr($sub,2);
	$pos=0;
	$num='';
	
	while(($sub[$pos]>='0')&&($sub[$pos]<='9')){
		$num.=$sub[$pos++];
	}
	
	$str=ereg_replace('\$r'.$num, ''.$Randoms[$num], $str);
    }
    
//    if (strstr($str,'$CL')&&($t=='res')) {
//	if (strtolower($clones) == 'no') {
//	    $str=ereg_replace('\$CL', '', $str);
//	} elseif ($crc<>'') {
//	}
	$str=ereg_replace('\$CL', $clon, $str);
//    }
    
    if($echo) echo $str; else return $str;
}

// -----------------------------------------------
// make_nav($query_orig)
// -----------------------------------------------
function make_nav($query_orig){
   global $found,$np,$isnext,$ps,$tag,$ul,$self,$o,$m,$cat;
   global $dt, $dp, $dx, $dm, $dy, $dd, $db, $de, $lang, $wm, $wf;
   global $q_local,$ul_local,$t_local,$db_local,$de_local,$lang_local;

   if($np>0){
     $prevp=$np-1;
     $prev_href="$self?q=$q_local&np=$prevp&m=$m".
                ($ps==20?'':"&ps=$ps").
                ($tag==''?'':"&t=$t_local").
                ($ul==''?'':"&ul=$ul_local").
		($wm==''?'':"&wm=$wm").
		($wf==''?'':"&wf=$wf").
                (!$o?'':"&o=$o").
                ($dt=='back'?'':"&dt=$dt").
                (!$dp?'':"&dp=$dp").
                (!$dx?'':"&dx=$dx").
                ($dd=='01'?'':"&dd=$dd").
                (!$dm?'':"&dm=$dm").
                ($dy=='1970'?'':"&dy=$dy").
                ($db=='01/01/1970'?'':"&db=$db_local").
                ($de=='31/12/2020'?'':"&de=$de_local").
                ($cat==''?'':"&cat=$cat").
                ($lang==''?'':"&lang=$lang_local");

     $nav_left="<TD><A HREF=\"$prev_href\">Prev</A></TD>\n";
   } elseif ($np==0) {
     $nav_left="<TD><FONT COLOR=\"#707070\">Prev</FONT></TD>\n";
   }

   if($isnext==1) {
     $nextp=$np+1;
     $next_href="$self?q=$q_local&np=$nextp&m=$m".
     		($ps==20?'':"&ps=$ps").
                ($tag==''?'':"&t=$t_local").
                ($ul==''?'':"&ul=$ul_local").
		($wm==''?'':"&wm=$wm").
		($wf==''?'':"&wf=$wf").
                (!$o?'':"&o=$o").
                ($dt=='back'?'':"&dt=$dt").
                (!$dp?'':"&dp=$dp").
                (!$dx?'':"&dx=$dx").
                ($dd=='01'?'':"&dd=$dd").
                (!$dm?'':"&dm=$dm").
                ($dy=='1970'?'':"&dy=$dy").
                ($db=='01/01/1970'?'':"&db=$db_local").
                ($de=='31/12/2020'?'':"&de=$de_local").
                ($cat==''?'':"&cat=$cat").
                ($lang==''?'':"&lang=$lang_local");

     $nav_right="<TD><A HREF=\"$next_href\">Next</TD>\n";
   } else {
     $nav_right="<TD><FONT COLOR=\"#707070\">Next</FONT></TD>\n";
   }

   $nav_bar0='<TD>$NP</TD>';
   $nav_bar1='<TD><A HREF="$NH">$NP</A></TD>';

   $tp=ceil($found/$ps);

   $cp=$np+1;

   if ($cp>5) {
      $lp=$cp-5;
   } else {
      $lp=1;
   }

   $rp=$lp+10-1;
   if ($rp>$tp) {
      $rp=$tp;
      $lp=$rp-10+1;
      if ($lp<1) $lp=1;
   }


   if ($lp!=$rp) {
      for ($i=$lp; $i<=$rp;$i++) {
         $realp=$i-1;
   
         if ($i==$cp) {
            $nav_bar=$nav_bar.$nav_bar0;
         } else {
            $nav_bar=$nav_bar.$nav_bar1;
         }
         
         $href="$self?q=$q_local&np=$realp&m=$m".
               	($ps==20?'':"&ps=$ps").
                ($tag==''?'':"&t=$t_local").
                ($ul==''?'':"&ul=$ul_local").
		($wm==''?'':"&wm=$wm").
		($wf==''?'':"&wf=$wf").
                (!$o?'':"&o=$o").
                ($dt=='back'?'':"&dt=$dt").
                (!$dp?'':"&dp=$dp").
                (!$dx?'':"&dx=$dx").
                ($dd=='01'?'':"&dd=$dd").
                (!$dm?'':"&dm=$dm").
                ($dy=='1970'?'':"&dy=$dy").
                ($db=='01/01/1970'?'':"&db=$db_local").
                ($de=='31/12/2020'?'':"&de=$de_local").
                ($cat==''?'':"&cat=$cat").
                ($lang==''?'':"&lang=$lang_local");

         $nav_bar=ereg_replace('\$NP',"$i",$nav_bar);
         $nav_bar=ereg_replace('\$NH',"$href",$nav_bar);
      }
   
      $nav="<TABLE BORDER=0><TR>$nav_left $nav_bar $nav_right</TR></TABLE>\n";
   } elseif ($found) {
      $nav="<TABLE BORDER=0><TR>$nav_left $nav_right</TR></TABLE>\n";
   }
   
   return $nav;
}

// -----------------------------------------------
//  M A I N 
// -----------------------------------------------

   if (preg_match("/^(\d+)\.(\d+)\.(\d+)/",phpversion(),$param)) {
	$phpver=$param[1];
   	if ($param[2] < 9) {
   		$phpver .= "0$param[2]";
   	} else {
   		$phpver .= "$param[2]";
   	}
        if ($param[3] < 9) {
   		$phpver .= "0$param[3]";
   	} else {
   		$phpver .= "$param[3]";
   	}
   } else {
   	print "Cannot determine php version: <b>".phpversion()."</b>\n";
   	exit;
   }
     
   $have_spell_flag=0;

   if (Udm_Api_Version() >= 30204) {
	$udm_agent=Udm_Alloc_Agent($dbaddr);	
   } else {
	$udm_agent=Udm_Alloc_Agent($dbaddr,$dbmode);	
   }       

   Udm_Set_Agent_Param($udm_agent,UDM_PARAM_PAGE_SIZE,$ps);
   Udm_Set_Agent_Param($udm_agent,UDM_PARAM_PAGE_NUM,$np);

   if ($phpver >= 40006) {
        if ($temp_cp_arr=Udm_Cat_Path($udm_agent,$cat)) {
	       	reset($temp_cp_arr);
	       	$temp_cp='';
	       	for ($i=0; $i<count($temp_cp_arr); $i+=2) {
	       		$cp_path=$temp_cp_arr[$i];
	       		$cp_name=$temp_cp_arr[$i+1];
	       		$temp_cp .= " &gt; <a href=\"$PHP_SELF?cat=$cp_path\">$cp_name</a> ";
	       	}
	       	$t_CP=$temp_cp;
	} 

	if ($temp_cp_arr=Udm_Cat_List($udm_agent,$cat)) {
	       	reset($temp_cp_arr);
	       	$temp_cp='';
	        for ($i=0; $i<count($temp_cp_arr); $i+=2) {
	       		$cp_path=$temp_cp_arr[$i];
	       		$cp_name=$temp_cp_arr[$i+1];
	       		$temp_cp .= "<a href=\"$PHP_SELF?cat=$cp_path\">$cp_name</a><br>";
	       	}
	        $t_CS=$temp_cp;
	}

        if ($temp_cp_arr=Udm_Cat_Path($udm_agent,$category)) {
	       	reset($temp_cp_arr);
	       	$temp_cp='';
	        for ($i=0; $i<count($temp_cp_arr); $i+=2) {
	      		$cp_path=$temp_cp_arr[$i];
	       		$cp_name=$temp_cp_arr[$i+1];
	       		$temp_cp .= " &gt; <a href=\"$PHP_SELF?cat=$cp_path\">$cp_name</a> ";
	       	}
	        $t_DY=$temp_cp;
	}
   }

   $trackquery=strtolower($trackquery);
   if ($trackquery == 'yes') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_TRACK_MODE,UDM_TRACK_ENABLED);
   } else {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_TRACK_MODE,UDM_TRACK_DISABLED);
   }

   $phrase=strtolower($phrase);
   if ($phrase == 'yes') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_PHRASE_MODE,UDM_PHRASE_ENABLED);
   } else {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_PHRASE_MODE,UDM_PHRASE_DISABLED);
   }

   $cache=strtolower($cache);
   if ($cache == 'yes') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_CACHE_MODE,UDM_CACHE_ENABLED);
   } else {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_CACHE_MODE,UDM_CACHE_DISABLED);
   }

   $ispelluseprefixes=strtolower($ispelluseprefixes);
   if ($ispelluseprefixes == 'yes') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_ISPELL_PREFIXES,UDM_PREFIXES_ENABLED);
   } else {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_ISPELL_PREFIXES,UDM_PREFIXES_DISABLED);
   }

   if (Udm_Api_Version() >= 30111) {
   	$crosswords=strtolower($crosswords);
   	if ($crosswords == 'yes') {
   		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_CROSS_WORDS,UDM_CROSS_WORDS_ENABLED);
   	} else {
   		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_CROSS_WORDS,UDM_CROSS_WORDS_DISABLED);
   	}
   }

   if ($localcharset != '') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_CHARSET,$localcharset);
   }
   
   if (Udm_Api_Version() >= 30200) {	   
   	if ($localcharset == '') Udm_Set_Agent_Param($udm_agent,UDM_PARAM_CHARSET,'utf-8');
   	if ($browsercharset != '') {
   		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_BROWSER_CHARSET,$browsercharset);
		Header ("Content-Type: text/html; charset=$browsercharset");
	} else {
		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_BROWSER_CHARSET,'utf-8');
		Header ("Content-Type: text/html; charset=utf-8");	
	}	
	if ($hlbeg != '') {
		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_HLBEG,$hlbeg);	
	} else {
	        $hlbeg='<font color="000088"><b>';
		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_HLBEG,'<font color="000088"><b>');	
	}
	if ($hlend != '') {
		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_HLEND,$hlend);	
	} else {
		$hlend='</b></font>';
		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_HLEND,'</b></font>');	
	}
   }


   for ($i=0; $i < count($stopwordtable_arr); $i++) {
   	if ($stopwordtable_arr[$i] != '') {
   		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_STOPTABLE,$stopwordtable_arr[$i]);
   	}
   }

   for ($i=0; $i < count($stopwordfile_arr); $i++) {
   	if ($stopwordfile_arr[$i] != '') {
   		Udm_Set_Agent_Param($udm_agent,UDM_PARAM_STOPFILE,$stopwordfile_arr[$i]);
   	}
   }

   if (Udm_Api_Version() >= 30203) {   	
        for ($i=0; $i < count($synonym_arr); $i++) {
   	    if ($synonym_arr[$i] != '') {
   	        Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SYNONYM,$synonym_arr[$i]);
   	    }
        }
	
	for ($i=0; $i < count($searchd_arr); $i++) {
   	    if ($searchd_arr[$i] != '') {
   	        Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SEARCHD,$searchd_arr[$i]);
   	    }
        }

	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_QSTRING,$QUERY_STRING);
        Udm_Set_Agent_Param($udm_agent,UDM_PARAM_REMOTE_ADDR,$REMOTE_ADDR);
   }

   if (Udm_Api_Version() >= 30204) {   	
	if ($have_query_flag)Udm_Set_Agent_Param($udm_agent,UDM_PARAM_QUERY,$query_orig);
   }

   if  ($m=='any') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SEARCH_MODE,UDM_MODE_ANY);
   } elseif ($m=='all') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SEARCH_MODE,UDM_MODE_ALL);
   } elseif ($m=='bool') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SEARCH_MODE,UDM_MODE_BOOL);
   } elseif ($m=='phrase') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SEARCH_MODE,UDM_MODE_PHRASE);
   } else {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_SEARCH_MODE,UDM_MODE_ALL);
   }

   if  ($wm=='wrd') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_WORD_MATCH,UDM_MATCH_WORD);
   } elseif ($wm=='beg') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_WORD_MATCH,UDM_MATCH_BEGIN);
   } elseif ($wm=='end') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_WORD_MATCH,UDM_MATCH_END);
   } elseif ($wm=='sub') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_WORD_MATCH,UDM_MATCH_SUBSTR);
   } else {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_WORD_MATCH,UDM_MATCH_WORD);
   }

   if ($minwordlength >= 0) {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_MIN_WORD_LEN,$minwordlength);
   }

   if ($maxwordlength >= 0) {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_MAX_WORD_LEN,$maxwordlength);	
   }
   
   if ($phpver >= 40007) {
   	if ($vardir != '') Udm_Set_Agent_Param($udm_agent,UDM_PARAM_VARDIR,$vardir);
	if ($datadir != '') Udm_Set_Agent_Param($udm_agent,UDM_PARAM_VARDIR,$datadir);
   }

   if ($wf != '') {
   	Udm_Set_Agent_Param($udm_agent,UDM_PARAM_WEIGHT_FACTOR,$wf);
   }

   if ($ul != '') {
   	$auto_wild=strtolower($auto_wild);   
   	if (($auto_wild == 'yes') || 
	    ($auto_wild == '')) {
   		if ((substr($ul,0,7) == 'http://') ||
		    (substr($ul,0,8) == 'https://') ||
		    (substr($ul,0,7) == 'news://') ||
		    (substr($ul,0,6) == 'ftp://')) {
		    	Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_URL,"$ul%");
		} else {
			Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_URL,"%$ul%");
		}
   	} else {
   		Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_URL,$ul);
   	}
   }
   
   if ($tag != '') Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_TAG,$tag);
   if ($cat != '') Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_CAT,$cat);
   if ($lang != '')Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_LANG,$lang);

   if (($dt == 'back') && ($dp != '0')) {
   		$recent_time=format_dp($dp);
   		if ($recent_time != 0) {
   			$dl=time()-$recent_time;
                        Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_DATE,">$dl");
   		}
   } elseif ($dt=='er') {
   		$recent_time=mktime(0,0,0,($dm+1),$dd,$dy);
   		if ($dx == -1) {
                        Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_DATE,"<$recent_time");
   		} elseif ($dx == 1) {
                        Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_DATE,">$recent_time");
   		}
   } elseif ($dt=='range') {
   		$begin_time=format_userdate($db);
   		if ($begin_time) Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_DATE,">$begin_time");

                $end_time=format_userdate($de);
   		if ($end_time) Udm_Add_Search_Limit($udm_agent,UDM_LIMIT_DATE,"<$end_time");
   }

//   if ($have_query_flag) $q=ParseQ($q);

?>

<HTML>
<HEAD>
 <TITLE>mnoGoSearch: <? echo HtmlSpecialChars(StripSlashes($query_orig)); ?></TITLE>
</HEAD>

<body BGCOLOR="#FFFFFF" LINK="#0050A0" VLINK="#0050A0" ALINK="#0050A0">
<center>

<FORM METHOD=GET ACTION="<? echo $self; ?>">
<table bgcolor=#eeeee0 border=0 width=100%>
<tr><td>
<BR>
<INPUT TYPE="hidden" NAME="ps" VALUE="10">
Search for: <INPUT TYPE="text" NAME="q" SIZE=50 VALUE="<? echo HtmlSpecialChars(StripSlashes($query_orig)); ?>">
<INPUT TYPE="submit" VALUE="Search!"><BR>

Results per page:
<SELECT NAME="ps">
<OPTION VALUE="10" <? if ($ps==10) echo 'SELECTED';?>>10
<OPTION VALUE="20" <? if ($ps==20) echo 'SELECTED';?>>20
<OPTION VALUE="50" <? if ($ps==50) echo 'SELECTED';?>>50
</SELECT>

Match:
<SELECT NAME="m">
<OPTION VALUE="all" <? if ($m=='all') echo 'SELECTED';?>>All
<OPTION VALUE="any" <? if ($m=='any') echo 'SELECTED';?>>Any
<OPTION VALUE="bool" <? if ($m=='bool') echo 'SELECTED';?>>Boolean
<OPTION VALUE="phrase" <? if ($m=='phrase') echo 'SELECTED';?>>Full phrase
</SELECT>


Search for:
<SELECT NAME="wm">
<OPTION VALUE="wrd" <? if ($wm=='wrd') echo 'SELECTED';?>>Whole word
<OPTION VALUE="beg" <? if ($wm=='beg') echo 'SELECTED';?>>Beginning
<OPTION VALUE="end" <? if ($wm=='end') echo 'SELECTED';?>>Ending
<OPTION VALUE="sub" <? if ($wm=='sub') echo 'SELECTED';?>>Substring
</SELECT>

<br>

Search through:
<SELECT NAME="ul">
<OPTION VALUE="" <? if ($ul=='') echo 'SELECTED';?>>Entire site
<OPTION VALUE="/docs/" <? if ($ul=='/docs/') echo 'SELECTED';?>>Docs
<OPTION VALUE="/files/" <? if ($ul=='/files') echo 'SELECTED';?>>Files
<OPTION VALUE="/servers/" <? if ($ul=='/servers/') echo 'SELECTED';?>>Servers
</SELECT>

in:
<SELECT NAME="wf">
<OPTION VALUE="222211" <? if ($wf=='222211') echo 'SELECTED';?>>all sections
<OPTION VALUE="220000" <? if ($wf=='220000') echo 'SELECTED';?>>Description
<OPTION VALUE="202000" <? if ($wf=='202000') echo 'SELECTED';?>>Keywords
<OPTION VALUE="200200" <? if ($wf=='200200') echo 'SELECTED';?>>Title
<OPTION VALUE="200010" <? if ($wf=='200010') echo 'SELECTED';?>>Body
</SELECT>

Language:
<SELECT NAME="lang">
<OPTION VALUE="" <? if ($lang=='222211') echo 'SELECTED';?>>Any
<OPTION VALUE="en" <? if ($lang=='en') echo 'SELECTED';?>>English
<OPTION VALUE="ru" <? if ($lang=='ru') echo 'SELECTED';?>>Russian
</SELECT>

Restrict search:
<SELECT NAME="t">
<OPTION VALUE="" <? if ($t=='') echo 'SELECTED';?>>All sites
<OPTION VALUE="AA" <? if ($t=='AA') echo 'SELECTED';?>>Sport
<OPTION VALUE="BB" <? if ($t=='BB') echo 'SELECTED';?>>Shopping
<OPTION VALUE="CC" <? if ($t=='CC') echo 'SELECTED';?>>Internet
</SELECT>

</td></tr>

<!-- 'search with time limits' options -->
<TR><TD>
<TABLE CELLPADDING=2 CELLSPACING=0 BORDER=0>
<CAPTION>
Limit results to pages published within a specified period of time.<BR>
<FONT SIZE=-1><I>(Please select only one option)</I></FONT>
</CAPTION>
<TR> 
<TD VALIGN=center><INPUT TYPE=radio NAME="dt" VALUE="back" <? if ($dt=='back') echo 'checked';?>></TD>
<TD><SELECT NAME="dp">
<OPTION VALUE="0" <? if ($dp=='0') echo 'SELECTED';?>>anytime
<OPTION VALUE="10M" <? if ($dp=='10M') echo 'SELECTED';?>>in the last ten minutes
<OPTION VALUE="1h" <? if ($dp=='1h') echo 'SELECTED';?>>in the last hour
<OPTION VALUE="7d" <? if ($dp=='7d') echo 'SELECTED';?>>in the last week
<OPTION VALUE="14d" <? if ($dp=='14d') echo 'SELECTED';?>>in the last 2 weeks
<OPTION VALUE="1m" <? if ($dp=='1m') echo 'SELECTED';?>>in the last month
</SELECT>
</TD>
</TR>
<TR>
<TD VALIGN=center><INPUT type=radio NAME="dt" VALUE="er" <? if ($dt=='er') echo 'checked';?>>
</TD>
<TD><SELECT NAME="dx">
<OPTION VALUE="1" <? if ($dx=='1') echo 'SELECTED';?>>After
<OPTION VALUE="-1" <? if ($dx=='-1') echo 'SELECTED';?>>Before
</SELECT>

or on

<SELECT NAME="dm">
<OPTION VALUE="0" <? if ($dm=='0') echo 'SELECTED';?>>January
<OPTION VALUE="1" <? if ($dm=='1') echo 'SELECTED';?>>February
<OPTION VALUE="2" <? if ($dm=='2') echo 'SELECTED';?>>March
<OPTION VALUE="3" <? if ($dm=='3') echo 'SELECTED';?>>April
<OPTION VALUE="4" <? if ($dm=='4') echo 'SELECTED';?>>May
<OPTION VALUE="5" <? if ($dm=='5') echo 'SELECTED';?>>June
<OPTION VALUE="6" <? if ($dm=='6') echo 'SELECTED';?>>July
<OPTION VALUE="7" <? if ($dm=='7') echo 'SELECTED';?>>August
<OPTION VALUE="8" <? if ($dm=='8') echo 'SELECTED';?>>September
<OPTION VALUE="9" <? if ($dm=='9') echo 'SELECTED';?>>October
<OPTION VALUE="10" <? if ($dm=='10') echo 'SELECTED';?>>November
<OPTION VALUE="11" <? if ($dm=='11') echo 'SELECTED';?>>December
</SELECT>
<INPUT TYPE=text NAME="dd" VALUE="<? echo $dd; ?>" SIZE=2 maxlength=2>
,
<SELECT NAME="dy" >
<OPTION VALUE="1999" <? if ($dy=='1999') echo 'SELECTED';?>>1999
<OPTION VALUE="2000" <? if ($dy=='2000') echo 'SELECTED';?>>2000
<OPTION VALUE="2001" <? if ($dy=='2001') echo 'SELECTED';?>>2001
</SELECT>
</TD>
</TR>
<TR>
<TD VALIGN=center><INPUT TYPE=radio NAME="dt" VALUE="range" <? if ($dt=='range') echo 'checked';?>>
</TD>
<TD>
Between
<INPUT TYPE=text NAME="db" VALUE="<? echo $db; ?>" SIZE=11 MAXLENGTH=11>
and
<INPUT TYPE=text NAME="de" VALUE="<? echo $de; ?>" SIZE=11 MAXLENGTH=11>
</TD>
</TR>
</TABLE>

</TD></TR>
<!-- end of stl options -->

<!-- categories stuff -->
<tr><td><? echo $t_CP; ?></td></tr>
<tr><td><? echo $t_CS; ?></td></tr>
<input type=hidden name=cat value="<? echo $cat; ?>">
<!-- categories stuff end -->

</table>
</form>
</center>


<?

   $ispellmode=strtolower($ispellmode);
   if ($ispellmode == 'db') {
   	if (! Udm_Load_Ispell_Data($udm_agent,UDM_ISPELL_TYPE_DB,'','',1)) {
		print_error_local(Udm_Error($udm_agent));
	} else $have_spell_flag=1;
   } elseif ($ispellmode == 'server') {
   	if (Udm_Api_Version() >= 30112) {
   		if (! Udm_Load_Ispell_Data($udm_agent,UDM_ISPELL_TYPE_SERVER,$spell_host,'',1)) {
			print_error_local("Error loading ispell data from server $spell_host");
		} else $have_spell_flag=1;
   	}
   } elseif ($ispellmode == 'text') {
   	reset($affix_file);
        while (list($t_lang,$file)=each($affix_file)) {
        	if (! Udm_Load_Ispell_Data($udm_agent,UDM_ISPELL_TYPE_AFFIX,$t_lang,$file,0)) {
        		print_error_local("Error loading ispell data from file");
        	} else $have_spell_flag=1;

        	$temp=$spell_file[$t_lang];
        	for ($i=0; $i<count($temp); $i++) {
        		if (! Udm_Load_Ispell_Data($udm_agent,UDM_ISPELL_TYPE_SPELL,$t_lang,$temp[$i],1)) {
        			print_error_local("Error loading ispell data from file");
        		} else $have_spell_flag=1;
        	} 
        }
   }

if (! $have_query_flag) {
    print_bottom();
    return;	
} elseif ($have_query_flag && ($q=='')) {
    print ("<FONT COLOR=red>You should give at least one word to search for.</FONT>\n"); 	
    print_bottom();
    return;
}         

$res=Udm_Find($udm_agent,$q);	

if(($errno=Udm_Errno($udm_agent))>0){
	print_error_local(Udm_Error($udm_agent));
} else {
	$found=Udm_Get_Res_Param($res,UDM_PARAM_FOUND);
        $rows=Udm_Get_Res_Param($res,UDM_PARAM_NUM_ROWS);
        $wordinfo=Udm_Get_Res_Param($res,UDM_PARAM_WORDINFO);
	$searchtime=Udm_Get_Res_Param($res,UDM_PARAM_SEARCHTIME);
	$first_doc=Udm_Get_Res_Param($res,UDM_PARAM_FIRST_DOC);
	$last_doc=Udm_Get_Res_Param($res,UDM_PARAM_LAST_DOC);

        if (!$found) {
        	print ("Search Time: $searchtime<br>Search results:\n");
		print ("<small>$wordinfo</small><HR><CENTER>Sorry, but search returned no results.<P>\n");
		print ("<I>Try to produce less restrictive search query.</I></CENTER>\n");

    		print_bottom();
    		return;
	} 

        $from=IntVal($np)*IntVal($ps); 
	$to=IntVal($np+1)*IntVal($ps); 

	if($to>$found) $to=$found;
	if (($from+$ps)<$found) $isnext=1;
	$nav=make_nav($query_orig);

        print("Search Time: $searchtime<br>Search results: <small>$wordinfo</small><HR>\n");
	print("Displaying documents $first_doc-$last_doc of total <B>$found</B> found.\n");

        $stored_link=-1;
	if ((Udm_Api_Version() == 30203) && ($storedaddr != '')) {
	    $stored_link=Udm_Open_Stored($udm_agent,$storedaddr);
	}
                        
        for($i=0;$i<$rows;$i++){
        	if (Udm_Api_Version() >= 30204) {
        		$excerpt_flag=Udm_Make_Excerpt($udm_agent, $res, $i);
        	}
	
		$ndoc=Udm_Get_Res_Field($res,$i,UDM_FIELD_ORDER);
		$rating=Udm_Get_Res_Field($res,$i,UDM_FIELD_RATING);
		$url=Udm_Get_Res_Field($res,$i,UDM_FIELD_URL);
		$contype=Udm_Get_Res_Field($res,$i,UDM_FIELD_CONTENT);
		$docsize=Udm_Get_Res_Field($res,$i,UDM_FIELD_SIZE);
		$lastmod=format_lastmod(Udm_Get_Res_Field($res,$i,UDM_FIELD_MODIFIED));
		
		$title=Udm_Get_Res_Field($res,$i,UDM_FIELD_TITLE);   
		$title=($title) ? htmlspecialChars($title):'No title';
		
		$title=ParseDocText($title);
		$text=ParseDocText(htmlspecialChars(Udm_Get_Res_Field($res,$i,UDM_FIELD_TEXT)));
		$keyw=ParseDocText(htmlspecialChars(Udm_Get_Res_Field($res,$i,UDM_FIELD_KEYWORDS)));
		$desc=ParseDocText(htmlspecialChars(Udm_Get_Res_Field($res,$i,UDM_FIELD_DESC)));

		$crc=Udm_Get_Res_Field($res,$i,UDM_FIELD_CRC);
		$rec_id=Udm_Get_Res_Field($res,$i,UDM_FIELD_URLID);
		
		if (Udm_Api_Version() >= 30203) {
		    $doclang=Udm_Get_Res_Field($res,$i,UDM_FIELD_LANG);
		    $doccharset=Udm_Get_Res_Field($res,$i,UDM_FIELD_CHARSET);
		}
		
		if ($phpver >= 40006) {
  			$category=Udm_Get_Res_Field($res,$i,UDM_FIELD_CATEGORY);
  		} else {
  			$category='';
  		}
  
		print ("<DL><DT><b>$ndoc.</b><a href=\"$url\" TARGET=\"_blank\"><b>$title</b></a>\n");
                print ("[<b>$rating</b>]<DD>\n");
                print (($desc != '')?$desc:$text."...<BR>$t_DY<UL><li>\n");
                print ("<A HREF=\"$url\" TARGET=\"_blank\">$url</A>\n");
                print ("($contype) $lastmod, $docsize bytes</UL></DL>\n");
		
		if (Udm_Api_Version() == 30203) {
		    if ((($stored_link>0) && (Udm_Check_Stored($udm_agent,$stored_link,Udm_CRC32($udm_agent,$url)))) ||
		        ($stored_link==-1)) {		    
		        $storedstr="$storedocurl?rec_id=".Udm_CRC32($udm_agent,$url).
            		    	"&DM=".urlencode($lastmod).
	    		    	"&DS=$docsize".
	    		    	"&L=$doclang".
	    		    	"&CS=$doccharset".
	    		    	"&DU=".urlencode($url).
	    		    	"&q=".urlencode($query_orig);
		    	print ("<DD><a href=\"$storedstr\">Cached copy</a>\n");
		    }
		} 		
	}	

        if ((Udm_Api_Version() == 30203) &&
	    ($stored_link>0)) {
	    Udm_Close_Stored($udm_agent, $stored_link);
	}
        
        print("<HR><CENTER> $nav </CENTER>\n");    
	print_bottom();

        // Free result
	Udm_Free_Res($res);
}

	if ($have_spell_flag) Udm_Free_Ispell_Data($udm_agent);
	Udm_Free_Agent($udm_agent);
?>
