<?php

/*********************************************************************
 *    Cyberlib - (C) American Metrocomm Internet Services            *
 *           by Timothy Whitfield <timothy@ametro.net>               *
 *                                                                   *
 *    PHP Cybercash API - This requires that CyberCash support be    *
 *    compiled.							     *
 *								     *
 *                                                                   *
 *    This is an attempt to duplicate the cybercash API for PHP     *
 *    users.                                                         *
 *********************************************************************
 *  This does not require merchant_conf for reasons that any file    *
 *  can be accessed by the web server can be accessed by any cgi.    *
 *********************************************************************
 */

 function SendCC2_1Server($merchant,$merchant_key,
                          $url,$operation,$CCNVList)
 {
   /* We need to make the url. */
   $url=$url."cr21api.cgi/".$operation;

   return SendCCServer($merchant,$merchant_key,$url,$CCNVList); 
 }
 
 function SendCCServer($merchant,$merchant_key,$url,$CCNVList)
 {
   /* Break the url into parts. */
   $url=parse_url($url);
   
   /* Turn the name value pairs into a url encoded message. */
   $pairs="";
   $done=0;
   $more=list($key,$val)=each($CCNVList);
   while(!$done)
   {
     $pairs.=chop($key)."=".urlencode(chop($val));
     
     $more=list($key,$val)=each($CCNVList);
     if($more)
     {
       $pairs.="&";
     }
     else
     {
       $done=1;
     }
   }

   $encrypted_pairs=CCEncrypt($merchant_key,$pairs);
   $pairs_length=strlen($encrypted_pairs);
 
   $message=sprintf("POST %s/%s HTTP/1.0\r\n",$url["path"],$merchant);
   $message.=sprintf("User-Agent: CCMCK-%s\r\n","3.2.0.5");
   $message.="Content-type: application/x-www-form-urlencoded\r\n";
   $message.=sprintf("Content-length: %d\r\n",$pairs_length);
   $message.="\r\n";
   $message.=$encrypted_pairs."\r\n";

$response=CCSocketSend($merchant_key,$url["host"],isset($url["port"])?$url["port"]:"",$message);
   return $response;
 }

 function CCEncrypt($merchant_key,$pairs)
 {
   $session_key=sprintf("%s #%ld",date("D M j H:i:s Y"),getmypid());
   $enc_msg=cybercash_encr($merchant_key,$session_key,$pairs);
   $pairs=cybercash_base64_encode($enc_msg["outbuff"]);
   $mac=cybercash_base64_encode($enc_msg["macbuff"]);
   
   /* This adds the information needed to decrypt. */
   $encrypted_pairs="message=".urlencode($pairs)."&";
   $encrypted_pairs.="session-key=".urlencode($session_key)."&";
   $encrypted_pairs.="mac=".urlencode($mac);
   
   
   return $encrypted_pairs;
 }
 
 function CCSocketSend($merchant_key,$host,$port,$message)
 {
 
   if(!$port)
   {
     $port="80";
   }
 
   /*This opens the port. */
   $fd=fsockopen($host,$port);

   /* Minor error checking. */
   if(!$fd)
   {
     $vars["MStatus"]="failure-hard";
     $vars["MErrMsg"]="Error contacting credit processor.";

     return $vars;
   }

   /*This sends the message. */
   fputs($fd,$message);
   
   /* We read the header in and parse at the same time. */

   /* Retrieve header. */
   $i=0;
   $response="";
   while(!feof($fd) && $response != "\n")
   {
     $response="";
     $more="";
     while(!feof($fd) && $more != "\n")
     {
       $more=fgetc($fd);
       if($more != "\n" || $response=="")
       {
         if($more != "\r")
         {
           $response.=$more;
         }
       }
     }
     $header[$i++]=$response;
   }

   /* We will now get the message. */
   $message="";
   while(!feof($fd))
   {
     $message.=fgets($fd,50);
   }
 
   /* It should be ok to close the socket now. */
   fclose($fd);
   
   /* This set of variables is encoded/encrypted. */
   $vars=CCGetVars($message);
   $vars["message"]=cybercash_base64_decode($vars["message"]);
   $vars["mac"]=cybercash_base64_decode($vars["mac"]);
   
   /* Check for errors with the request/decryption. */
   /* message is base64 and encrypted. */
   $dec_msg=cybercash_decr($merchant_key,$vars["session-key"],
                     $vars["message"]);

   if($dec_msg["errcode"])
   {
     $vars["MStatus"]="failure-hard";
     $vars["MErrMsg"]="Response non-decodable.";
     return $vars;
   }

   if($dec_msg["macbuff"] != $vars["mac"])
   {
     $vars["MStatus"]="failure-hard";
     $vars["MErrMsg"]="Signitures do not match.";
     return $vars;
   }
   
   /* We will have to parse again to get more info. */
   $vars=CCGetVars($dec_msg["outbuff"]);
   
   return $vars;
 }
 
 function CCGetVars($message)
 {
   /* Retrieve variables. 
      This function retrieves variables in var/value key pairs.
      So that $myvar["var"]==value
   */
   
   /* Need to find these variables too. */
   $cx=0;
   $response="";
   $more="";
   $message.="\n";
   $msg_len=strlen($message);

   while($cx<=$msg_len)
   {
     $more="";
     $varname="";
     $varval="";
     while($cx<=$msg_len && $more != "&" && $more != "\n")
     {
       $more=substr($message,$cx,1);
       $cx++;
       if($more != "&" && $more != "=" && $more != "\n")
       {
         $response=$response.$more;
       }
       if($more=="=")
       {
         $varname=$response;
         $response="";
       }
       if($more=="&" || $more=="\n")
       {
         $varval=$response;
         $response="";
       }
     }
     
     if($varname != "")
     {
       $cybervar[$varname]=urldecode($varval);
     }
   }
  
   return $cybervar;
 }
?>
