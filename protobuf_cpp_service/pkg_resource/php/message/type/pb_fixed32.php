<?php
/**
 * @author Nikolai Kordulla
 */
class PB32Bit extends PBScalar
{
	var $wired_type = PBMessage::WIRED_32BIT;

	/**
	 * Parses the message for this type
	 *
	 * @param array
	 */
	public function ParseFromArray()
	{
		// just extract the string
                $pointer = $this->reader->get_pointer();
                $this->reader->add_pointer(4);
                $this->value = $this->reader->get_message_from($pointer);

		$value = unpack("V",$this->value);
		$this->value = $value[1];
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
		$string .= pack("V",$this->value);
		return $string;
	}
}
?>
