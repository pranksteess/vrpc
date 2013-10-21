#!/usr/local/services/php-5.2.14/bin/php
<?php
    require_once 'pb_service_sample.php';
    $serverConfig = array(
	'infoServer' => array(
		//'modid' => '108033', 'cmdid' => '327680',
		'ip' => '172.27.202.79', 'port' => 18667,
	)
    );
    PricingService::set_server($serverConfig['infoServer']);
    $req = new ReqGetItemInfo();
    $req->set_uint64_uin(454306603);
    $req->set_uint32_item_id(10022);
    $req->set_uint32_plat_id(1);
    $req->set_uint32_price_lock("1");
    $resp = PricingService::query($req);
    echo "int32_ret ".($resp->int32_ret())."\n";
    $c = $resp->rpt_msg_pay_info_size();
    for($i=0;$i<$c;$i++)
    {
        $payInfo = $resp->rpt_msg_pay_info($i);
	echo "uint32_days ".($payInfo->uint32_days())."\n";
	echo "uint32_price ".($payInfo->uint32_price())."\n";
	echo "uint32_discount ".($payInfo->uint32_discount())."\n";
	echo "uint32_pay_flag ".($payInfo->uint32_pay_flag())."\n";
	echo "str_pay_token ".($payInfo->str_pay_token())."\n";
	echo "str_product_id ".($payInfo->str_product_id())."\n";
    }
    echo "uint32_pay_info_default_index ".($resp->uint32_pay_info_default_index())."\n";
    echo "uint32_item_type ".($resp->uint32_item_type())."\n";
?>
