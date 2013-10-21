<?php
// just create from the proto file a pb_prot[NAME].php file
if(!isset($argv[1]) || !isset($argv[2]) || !isset($argv[3]))
{
	echo "usage: $argv[0] file.proto output_dir pkg_dir\n";
	exit(1);
}
$filename = $argv[1];
$output_dir =  $argv[2];
$pkg_dir =  $argv[3];
if(!file_exists($filename)) 
{
	echo "$filename not exists\n";
	exit(1);
}    

require_once($pkg_dir.'parser/pb_parser.php');

$test = new PBParser();
$test->parse($filename);

$fname = split("/", $filename);
$fname = $fname[count($fname) - 1];
$name = split('\.', $fname);
array_pop($name);
$name = join($name, '.');

$path = $output_dir;
/*if(!is_dir($path)) 
{ 
	mkdir($path, 0777, true);
}*/
$f1 = './pb_proto_' . $name . '.php';
$f2 = './pb_service_' . $name . '.php';
if(file_exists($f1))
{
	copy($f1, $path."/$f1");
	unlink("$f1");
}
if(file_exists($f2))
{
	copy($f2, $path."/$f2");
	unlink("$f2");
}
exit(0);
?>
