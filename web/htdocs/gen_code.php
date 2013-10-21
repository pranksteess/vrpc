<?php
define( "PROTO_FILE_NAME", "protofile" );
define( "PROTO_MAX_SIZE", 65535 );
define( "VRPC_GEN_CMD", "/usr/local/services/vrpc_gen-2.3/shell/spp_service_gen.sh" );
define( "VRPC_PROJ_PATH", "/tmp" );

//var_dump( $_FILES );

function CheckFile( $file, &$err )
{
   if( $file["error"] != 0 )
   {
       $err = "upload file error:" . $file["error"];
       return -10;
   }

   if( $file["size"] > 65535 )
   {
       $err = "file size too large";
       return -20;
   }

   return 0;
}

function MoveFile( $file, &$proto_file, &$err )
{
    $proto_file = "/tmp/". $file["name"];
    if( move_uploaded_file( $file["tmp_name"], $proto_file ) )
    {
        return 0;
    }
    else
    {
        $err = "move_uploaded_file err";
        return -1;
    }
}

function GenCode( $file, $proj_name, &$proj_full_path )
{
    $proj_full_path = VRPC_PROJ_PATH."/$proj_name";

    exec( "rm -rf $proj_full_path" );

    $cmd = VRPC_GEN_CMD . " " . dirname($file) . " $file $proj_name " . VRPC_PROJ_PATH;

    $last_line = exec( $cmd, $output, $retcode );
    if( $retcode != 0 )
    {
        error_log( "GenCode exec failed. retcode:$retcode", 0 );
        foreach( $output as $info )
        {
            error_log("output: $info", 0);
        }
        return -1;
    }

    return $retcode;
}

function ZipCode( $proj_name, $proj_full_path, &$zip_file )
{
    $zip_file = "/tmp/$proj_name.tar.gz";
    $cmd = "tar czvf $zip_file $proj_full_path";
//    echo "$cmd<br/>";
    $last_line = exec( $cmd, $output, $retcode );
//    echo "zip system ret:$ret retcode:$retcode<br/>";
    return $retcode;
}

function Output( $zip_file )
{
    $file = fopen( $zip_file, "r" );

    Header("Content-type: application/octet-stream");
    Header("Accept-Ranges: bytes");
    Header("Accept-Length: ".filesize($zip_file));
    Header("Content-Disposition: attachment; filename=" . basename($zip_file));

    echo fread($file,filesize($zip_file));
    fclose($file);
    
    return 0;
}

function GotoErrorPage( $err )
{
    echo <<<EOF
    
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<title>VRPC系统</title>

<link href="https://hrc.tencent.com/css/all.css" type="text/css" rel="stylesheet" />
<link href="https://hrc.tencent.com/css/guide.css" type="text/css" rel="stylesheet" />
<script src="https://hrc.tencent.com/js/jquery-1.26.js" type="text/javascript"></script>
<script src="https://hrc.tencent.com/js/jquery.util.js" type="text/javascript"></script>
<script src="https://hrc.tencent.com/js/jquery.drag.js" type="text/javascript"></script>
<script src="https://hrc.tencent.com/js.php?v=1367770080" type="text/javascript"></script>
<script src="https://hrc.tencent.com/js/min_template.js?max_age=86409" type="text/javascript"></script>

</head>

<body id="page_index">

     <div id="hd">
       <div class="logo clearfix">
            <h1 class=""><a href="index.php"><img src="images/logo.png"/></a></h1>
            <div class="right textr" id="headerr">
                            </div>
            <div class="clr"></div>
        </div>
    </div><div id="bread">
    &nbsp;
</div>
<div id="content">
    <table class="width1000" id="loginPage">
    
        <tbody>
    
            <tr>            
    
                <td width="98%">            
    
                <div class="common">            
    
                <div class="blueborder">
    
                <div align="center">
                <div id="ie6show"></div>
                出错: $err
                </div>
    
                </div>          
    
                </div>          
    
                </td>
    
            </tr>
    
        </tbody>
    
    </table>
</div>
<div class="width1000" id="footer">
    <div class="real-wrap mainarea" style="text-align:center;">
        <p>Copyright &copy; 1998-2013 Tencent. All Rights Reserved.</p>
    </div>
</div></body>

</html>

EOF;
    exit(1);
}

$strErr = "";
$proj_name = $_POST["projname"];
//echo "projname:" . $_POST["projname"] . "\n";

$iRet = CheckFile( $_FILES[PROTO_FILE_NAME], $strErr );
if( $iRet != 0 )
{
    GotoErrorPage( $strErr );
}

$iRet = MoveFile( $_FILES[PROTO_FILE_NAME], $proto_file, $strErr );
if( $iRet != 0 )
{
    GotoErrorPage( $strErr );
}

$proj_full_path = "";
$iRet = GenCode( $proto_file, $proj_name, $proj_full_path );
if( $iRet != 0 )
{
    GotoErrorPage( "生成代码出错，请确认proto文件格式是否正确. 错误码:$iRet" );
}

$zip_file = "";
$iRet = ZipCode( $proj_name, $proj_full_path, $zip_file );

Output( $zip_file );

