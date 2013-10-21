<?php
/**
 * @author Nikolai Kordulla
 */
class PB64Bit extends PBScalar
{
	var $wired_type = PBMessage::WIRED_64BIT;

	/**
	 * Parses the message for this type
	 *
	 * @param array
	 */
	public function ParseFromArray()
	{
		// just extract the string
                $pointer = $this->reader->get_pointer();
                $this->reader->add_pointer(8);
                $this->value = $this->reader->get_message_from($pointer);

		list ($hi,$lo) = array_values(unpack("V*V*",$this->value));
		if ($hi <0) $hi += (1 << 32);
		if ($lo <0) $lo += (1 << 32);
		$this->value = ($hi << 32) + $lo;
	}

	/**
	 * Serializes type
	 */
	public function SerializeToString($rec=-1)
	{
                $string = '';

                if ($rec > -1)
                {
                        $string .= $this->base128->set_value($rec << 3 | $this->wired_type);
                }		
		$string .= pack("VV",$this->value >> 32,$this->value&0xFFFFFFFF);
		return $string;
	}
}
?>
