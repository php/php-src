<!-- 
    $Source$
    $Id$ 
-->

<html>
<body>

<form method=post>
<input type=text size=30 name=q value="<? echo $q; ?>">
<input type=submit value=" Search ! ">
</form>

<pre>

<?
	if ($q=='') {
		print("</pre></body></html>\n");
		exit();
	}
	
// Stage 1: allocate UdmSearch agent, set DBAddr and DBMode
// DBMode is optional, "single" by default

	$udm=Udm_Alloc_Agent("mysql://udm:udm@localhost/udm/",'crc-multi');
	
// Stage 2: set search parameters

	$page_size=10;
	$page_number=0;
	
	Udm_Set_Agent_Param($udm,UDM_PARAM_PAGE_SIZE,$page_size);
	Udm_Set_Agent_Param($udm,UDM_PARAM_PAGE_NUM,$page_number);
	Udm_Set_Agent_Param($udm,UDM_PARAM_SEARCH_MODE,UDM_MODE_BOOL);
	Udm_Set_Agent_Param($udm,UDM_PARAM_CACHE_MODE,UDM_CACHE_DISABLED);
	Udm_Set_Agent_Param($udm,UDM_PARAM_TRACK_MODE,UDM_TRACK_DISABLED);
	Udm_Set_Agent_Param($udm,UDM_PARAM_CHARSET,"koi8-r");
	Udm_Set_Agent_Param($udm,UDM_PARAM_STOPTABLE,"stopwords");
//	Udm_Set_Agent_Param($udm,UDM_PARAM_STOPFILE,"stop.txt");
	
// Stage 3: perform search 

	$res=Udm_Find($udm,$q);	

// Stage 4: display results

	// Check error code
	if(($errno=Udm_Errno($udm))>0){
		// Display error message
		printf("Error #%d: '%s'\n",$errno,Udm_Error($udm));
	}else{
		$first=$page_size*$page_number+1;
		
		// Get result parameters
		$total=Udm_Get_Res_Param($res,UDM_PARAM_FOUND);
		$rows=Udm_Get_Res_Param($res,UDM_PARAM_NUM_ROWS);

		printf("Documents %d-%d from %d total found\n\n",
			$first,$first+$rows-1,$total);

		// Fetch all rows
		for($i=0;$i<$rows;$i++){
			printf("%3d. %s\n",$first+$i,Udm_Get_Res_Field($res,$i,UDM_FIELD_URL));
			printf("     CONT : %s\n",htmlspecialchars(Udm_Get_Res_Field($res,$i,UDM_FIELD_CONTENT)));
			printf("     TITLE: %s\n",htmlspecialchars(Udm_Get_Res_Field($res,$i,UDM_FIELD_TITLE)));
			printf("     KEYWORDS: %s\n",htmlspecialchars(Udm_Get_Res_Field($res,$i,UDM_FIELD_KEYWORDS)));
			printf("     DESC: %s\n",htmlspecialchars(Udm_Get_Res_Field($res,$i,UDM_FIELD_DESC)));
			printf("     TEXT: %s\n",htmlspecialchars(Udm_Get_Res_Field($res,$i,UDM_FIELD_TEXT)));
			printf("     SIZE : %d\n",Udm_Get_Res_Field($res,$i,UDM_FIELD_SIZE));
			printf("     MODIFIED : %s\n",Udm_Get_Res_Field($res,$i,UDM_FIELD_MODIFIED));
			printf("     URLID : %d\n",Udm_Get_Res_Field($res,$i,UDM_FIELD_URLID));
			printf("     SCORE : %d\n",Udm_Get_Res_Field($res,$i,UDM_FIELD_SCORE));
			printf("---------\n");
		}
				
		// Free result
		Udm_Free_Res($res);
	}

//Stage 5: free UdmSearch agent

	Udm_Free_Agent($udm);
?>

</pre>
</body>
</html>
