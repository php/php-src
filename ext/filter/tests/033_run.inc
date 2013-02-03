<?php
if (function_exists('setlocale')) {
	setlocale(LC_ALL, 'C');
}

function test($data) {
  return strtoupper($data);
}

$data = array(
	"PHP",
	"1",
	"foo@bar.com",
	"http://a.b.c",
	"1.2.3.4",
	"123",
	"123abc<>()",
	"O'Henry",
	"하퍼",
	"aa:bb:cc:dd:ee:ff",
);


foreach(filter_list() as $filter) {
	if($filter=="validate_regexp") {
		foreach($data as $k=>$d) $result[$k] = filter_var($d,filter_id($filter),array("options"=>array("regexp"=>'/^O.*/')));
	} else {
		foreach($data as $k=>$d) $result[$k] = filter_var($d,filter_id($filter),array("options"=>"test"));
	}
	printf("%-20s",$filter);
	printf("%-5s",$result[0]);
	printf("%-3s",$result[1]);
	printf("%-15s",$result[2]);
	printf("%-20s",$result[3]);
	printf("%-10s",$result[4]);
	printf("%-5s",$result[5]);
	printf("%-20s",$result[6]);
	printf("%-15s",$result[7]);
	printf("%-10s",$result[8]);
	printf("%-10s\n",$result[9]);
}
?>
