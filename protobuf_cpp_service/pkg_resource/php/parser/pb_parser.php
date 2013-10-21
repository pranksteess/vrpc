<?php
/**
 * Parse a .proto file and generates the classes in a file
 * @author Nikolai Kordulla
 */
class PBParser
{
    // the message types array of (field, param[]='repeated,required,optional')
    var $m_types = array();
    
    // the message classtype
    var $c_types = array();
    
    // different types
    var $scalar_types = array('double', 'float', 'int32' => 'PBInt', 'int64' => 'PBInt',
                              'uint32' => 'PBInt', 'uint64' => 'PBInt', 'sint32' => 'PBSignedInt', 'sint64' => 'PBSignedInt',
                              'fixed32'=>'PB32Bit', 'fixed64'=>'PB64Bit', 'sfixed32'=>'PB32Bit', 'sfixed64'=>'PB64Bit',
                              'bool' => 'PBBool', 'string' => 'PBString', 'bytes' => 'PBString');

    /**
     * parses the profile and generates a filename with the name
     * pb_proto_[NAME]
     * @param String $protofile - the protofilename with the path
     */
    public function parse($protofile)
    {
        $string = file_get_contents($protofile);
        // now take the filename
        //$filename = str_replace("\\", "/", $filename);
        $filename = split("/", $protofile);
        $filename = $filename[count($filename) - 1];
        // strip the comments out of the protofile
        $this->_strip_comments($string);
        $string = trim($string);
        $this->_parse_message_type($string, '');
        unset($this->m_types[count($this->m_types)-1]);
        //$this->m_types = $this->m_types[0]['value'];
        // now create file with classes
        $name = split('\.', $filename);
        array_pop($name);
        $name = join($name, '.');
        $this->_create_class_file($name);
		$this->_create_service_file($name);
    }

    /**
     * Creates php class file for the proto file
     *
     * @param String $filename - the filename of the php file
     */
    private function _create_class_file($name)
    {
        $filename = 'pb_proto_' . $name . '.php';
		$string = '';
        foreach ($this->m_types as $classfile)
        {
            $classname = str_replace(".", "_", $classfile['name']);

            if ($classfile['type'] == 'message')
            {
                $string .= 'class ' . $classname  . " extends PBMessage\n{\n";
                $this->_create_class_constructor($classfile['value'], $string, $classname);
                $this->_create_class_body($classfile['value'], $string, $classname);
                $this->c_types[$classfile['name']] = 'PBMessage';
            }
            else if ($classfile['type'] == 'enum')
            {
                $string .= 'class ' . $classname  . " extends PBEnum\n{\n";
                $this->_create_class_definition($classfile['value'], $string);
                $this->c_types[$classfile['name']] = 'PBEnum';
            }
			else
			{
				continue;
			}
            // now create the class body with all set and get functions

            $string .= "}\n";
        }
        file_put_contents($filename, '<?php' . "\n" . $string . '?>');
    }
	
	/**
	 * Gets the type
	 * @param field array
	 * @return type
	 */
	private function _get_type($field)
	{
		if (isset($this->scalar_types[$field['value']['type']]))
			return $this->scalar_types[$field['value']['type']];
		else if (isset($this->c_types[$field['value']['namespace']]))
			return $this->c_types[$field['value']['namespace']];
		return $this->c_types[$field['value']['type']];
	}
	
    /**
     * Creates the class body with functions for each field
     * @param Array $classfile
     * @param String $string
     * @param String $classname - classname
     */
    private function _create_class_body($classfile, &$string, $classname)
    {
        foreach($classfile as $field)
        {
        	$type = $this->_get_type($field);
        	//var_dump($type);
        	//$type = $this->_get_type($field['value']['type']);
			if ( isset($field['value']['repeated']) && ( isset($this->scalar_types[$field['value']['type']]) 
			    										|| $type == 'PBEnum') )
			{
                $string .= '  function ' . $field['value']['name'] . '($offset)' . "\n  {\n";
                $string .= '    $v = $this->_get_arr_value("' . $field['value']['value'] . '", $offset);'  . "\n";
                $string .= '    return $v->get_value();' . "\n";;
                $string .= "  }\n";

                $string .= '  function append_' .  $field['value']['name'] . '($value)' . "\n  {\n";
                $string .= '    $v = $this->_add_arr_value("' . $field['value']['value'] . '");'  . "\n";
                $string .= '    $v->set_value($value);' . "\n";;
                $string .= "  }\n";

                $string .= '  function set_' .  $field['value']['name'] . '($index, $value)' . "\n  {\n";
                $string .= '    $v = new $this->fields["' . $field['value']['value'] . '"]();' . "\n";
                $string .= '    $v->set_value($value);' . "\n";
                $string .= '    $this->_set_arr_value("' . $field['value']['value'] . '", $index, $v);'  . "\n";
                $string .= "  }\n";

                $string .= '  function remove_last_' .  $field['value']['name'] . '()' . "\n  {\n";
                $string .= '    $this->_remove_last_arr_value("' . $field['value']['value'] . '");'  . "\n";
                $string .= "  }\n";

                $string .= '  function ' . $field['value']['name'] . '_size()' . "\n  {\n";
                $string .= '    return $this->_get_arr_size("' . $field['value']['value'] . '");'  . "\n";
                $string .= "  }\n";
			}			
            else if (isset($field['value']['repeated']))
            {
                $string .= '  function ' . $field['value']['name'] . '($offset)' . "\n  {\n";
                $string .= '    return $this->_get_arr_value("' . $field['value']['value'] . '", $offset);'  . "\n";
                $string .= "  }\n";

                $string .= '  function add_' .  $field['value']['name'] . '()' . "\n  {\n";
                $string .= '    return $this->_add_arr_value("' . $field['value']['value'] . '");'  . "\n";
                $string .= "  }\n";

                $string .= '  function set_' .  $field['value']['name'] . '($index, $value)' . "\n  {\n";
                $string .= '    $this->_set_arr_value("' . $field['value']['value'] . '", $index, $value);'  . "\n";
                $string .= "  }\n";

                $string .= '  function remove_last_' .  $field['value']['name'] . '()' . "\n  {\n";
                $string .= '    $this->_remove_last_arr_value("' . $field['value']['value'] . '");'  . "\n";
                $string .= "  }\n";

                $string .= '  function ' . $field['value']['name'] . '_size()' . "\n  {\n";
                $string .= '    return $this->_get_arr_size("' . $field['value']['value'] . '");'  . "\n";
                $string .= "  }\n";
            }
            else
            {
                $string .= '  function ' . $field['value']['name'] . "()\n  {\n";
                $string .= '    return $this->_get_value("' . $field['value']['value'] . '");'  . "\n";
                $string .= "  }\n";

                $string .= '  function set_' .  $field['value']['name'] . '($value)' . "\n  {\n";
                $string .= '    return $this->_set_value("' . $field['value']['value'] . '", $value);'  . "\n";
                $string .= "  }\n";
            }
        }
    }

    /**
     * Creates the class definitions
     * @param Array $classfile
     * @param String $string
     */
    private function _create_class_definition($classfile, &$string)
    {
        foreach($classfile as $field)
        {
            $string .= '  const ' . $field['0'] . '  = ' . $field['1'] . ";\n";
        }

    }


    /**
     * Creates the class constructor
     * @param Array $classfile
     * @param String $string
     * @param String $classname - classname
     */
    private function _create_class_constructor($classfile, &$string, $classname)
    {
        $string .= '  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;' . "\n";
        $string .= "  public function __construct(" . '$reader=null'  . ")\n  {\n";
        $string .= "    parent::__construct(" . '$reader'  . ");\n";

        foreach($classfile as $field)
        {
            $classtype = "";
            $classtype = $field['value']['type'];
            $classtype = str_replace(".", "_", $classtype);
            $_classtype = $classtype;
            // create the right namespace
            if (isset($this->scalar_types[strtolower($classtype)]))
                $classtype = $this->scalar_types[$classtype];
            else if ((strpos($classtype, '_') === false))
                $classtype = str_replace('.', '_', $field['value']['namespace']);


            $string .= '    $this->fields["' . $field['value']['value'] . '"] = "' . $classtype . '"' . ";\n";

            if (isset($field['value']['repeated']))
            {
                $string .= '    $this->values["' . $field['value']['value'] . '"] = array()' . ";\n";
            }
            else
            {
                //$string .= '    $this->fields["' . $field['value']['value'] . '"] = new ' . $classtype . "();\n";
                $string .= '    $this->values["' . $field['value']['value'] . '"] = ""' . ";\n";
            }

            // default value only for optional fields
            if (!isset($field['value']['repeated']) && isset($field['value']['optional'])
                    && isset($field['value']['default']))
            {
                $string .= '    $this->values["' . $field['value']['value'] . '"] = new ' . $classtype . "();\n";
                if (isset($this->scalar_types[strtolower($_classtype)]))
                    $string .= '    $this->values["' . $field['value']['value'] . '"]->value = ' . $field['value']['default'] . '' . ";\n";
                // it must be an enum field perhaps type check
                else
                    $string .= '    $this->values["' . $field['value']['value'] . '"]->value = ' . $classtype . '::' . $field['value']['default'] . '' . ";\n";
            }
        }
        $string .= "  }\n";
    }


    /**
     * Parses the message
     * @param String $string the proton file as string
     */
    private function _parse_message_type(&$string, $m_name, $path = '')
    {
        $myarray = array();
        $string = trim($string);
        if ($string == '')
            return;

        //var_dump($m_name);

        while (strlen($string) > 0)
        {
            $next = ($this->_next($string));
            if (strtolower($next) == 'message')
            {
                $string = trim(substr($string, strlen($next)));
                $name = $this->_next($string);

                $offset = $this->_get_begin_end($string, "{", "}");
                // now extract the content and call parse_message again
                $content = trim(substr($string, $offset['begin'] + 1, $offset['end'] - $offset['begin'] - 2));
                $this->_parse_message_type($content, $name, trim($path . '.' . $name, '.'));

                $string = '' . trim(substr($string, $offset['end']));
            }
            else if (strtolower($next) == 'service')
            {
                $string = trim(substr($string, strlen($next)));
                $name = $this->_next($string);

                $offset = $this->_get_begin_end($string, "{", "}");
                // now extract the content and call parse_message again
                $content = trim(substr($string, $offset['begin'] + 1, $offset['end'] - $offset['begin'] - 2));

				$this->m_types[] =  array('name' =>  trim($path . '.' . $name, '.'), 
											'type' => 'service', 'value' => $this->_parse_service($content));
                $string = '' . trim(substr($string, $offset['end']));
            }			
            else if (strtolower($next) == 'enum')
            {
                $string = trim(substr($string, strlen($next)));
                $name = $this->_next($string);
                $offset = $this->_get_begin_end($string, "{", "}");
                // now extract the content and call parse_message again
                $content = trim(substr($string, $offset['begin'] + 1, $offset['end'] - $offset['begin'] - 2));
                // now adding all to myarray
                $this->m_types[] = array('name' => trim($path . '.' . $name, '.'),
                                             'type' => 'enum', 'value' => $this->_parse_enum($content));
                // removing it from string
                $string = '' . trim(substr($string, $offset['end']));
            }
            else
            {
                // now a normal field
                $match = preg_match('/(.*);\s?/', $string, $matches, PREG_OFFSET_CAPTURE);
                if (!$match)
                    throw new Exception('Proto file missformed');
                $myarray[] = array('type' => 'field', 'value' => $this->_parse_field($matches[0][0], $myarray, $path));
                $string = trim(substr($string, $matches[0][1] + strlen($matches[0][0])));
            }
        }

        // now adding myarray to array
        $this->m_types[] =  array('name' => $path , 'type' => 'message', 'value' => $myarray);
    }

    /**
     * Parses a normal field
     * @param String $content - content
     */
    private function _parse_field($content, $array, $path)
    {
        $myarray = array();

        // parse the default value
        $match = preg_match('/\[\s?default\s?=\s?([^\[]*)\]\s?;/', $content, $matches, PREG_OFFSET_CAPTURE);
        if ($match)
        {
            $myarray['default'] = $matches[1][0];
            $content = trim(substr($content, 0, $matches[0][1])) . ';';
        }

        // parse the value
        $match = preg_match('/=\s(.*);/', $content, $matches, PREG_OFFSET_CAPTURE);
        if ($match)
        {
            $myarray['value'] = trim($matches[1][0]);
            $content = trim(substr($content, 0, $matches[0][1]));
        }
        else
            throw new Exception('Protofile no value at ' . $content);

        // parse all modifier
        $content = trim(trim(trim($content), ';'));
        $typeset = false;
        while (strlen($content) > 0)
        {
            $matches = $this->_next($content, true);
            $name = $matches[0][0];
            if (strtolower($name) == 'optional')
                $myarray['optional'] = true;
            else if (strtolower($name) == 'required')
                $myarray['required'] = true;
            else if (strtolower($name) == 'repeated')
                $myarray['repeated'] = true;
            else if ($typeset == false)
            {
                $type = $this->_check_type($name, $array, $path);
                $myarray['type'] = $type[0];
                $myarray['namespace'] = $type[1];
                $typeset = true;
            }
            else
            {
                $myarray['name'] = $name;
            }
            $content = trim(substr($content, strlen($name)));
        }

        return $myarray;
    }


    /**
     * Checks if a type exists
     * @param String $type - the type
     */
    private function _check_type($type, $array, $path)
    {
        if (isset($this->scalar_types[strtolower($type)]))
            return array(strtolower($type), '');

        // absolute or relative thing
        // calculate namespace
        $namespace = '';
        $namespace = $type;

        $apath = split("\.", $path);
        if ($apath > 1)
        {
            array_pop($apath);
            $namespace = trim(trim(join($apath, '.'), '.') . '.' . $type, '.');
        }

        // try the namespace
        foreach ($this->m_types as $message)
        {
            if ($message['name'] == $namespace)
            {
                return array($type, $namespace);
            }
        }

        // now try one deeper
        $namespace  = trim($path . '.' . $namespace, '.');
        foreach ($this->m_types as $message)
        {
            if ($message['name'] == $namespace)
            {
                return array($type, $namespace);
            }
        }

        // @TODO TYPE CHECK
        throw new Exception('Protofile type ' . $type . ' unknown!');
    }

    /**
     * Parses enum
     * @param String $content content of the enum
     */
    private function _parse_enum($content)
    {
        $myarray = array();
        $match = preg_match_all('/(.*);\s?/', $content, $matches);
        if (!$match)
            throw new Execption('Semantic error in Enum!');
        foreach ($matches[1] as $match)
        {
            $split = split("=", $match);
            $myarray[] = array(trim($split[0]), trim($split[1]));
        }
        return $myarray;
    }

    /**
     * Gets the next String
     */
    private function _next($string, $reg = false)
    {
        $match = preg_match('/([^\s^\{}]*)/', $string, $matches, PREG_OFFSET_CAPTURE);
        if (!$match)
            return -1;
        if (!$reg)
            return (trim($matches[0][0]));
        else
            return $matches;
    }

    /**
     * Returns the begin and endpos of the char
     * @param String $string protofile as string
     * @param String $char begin element such as '{'
     * @param String $charend end element such as '}'
     * @return array begin, end
     */
    private function _get_begin_end($string, $char, $charend)
    {
        $offset_begin = strpos($string, $char);

        if ($offset_begin === false)
            return array('begin' => -1, 'end' => -1);

        $_offset_number = 1;
        $_offset = $offset_begin + 1;
        while ($_offset_number > 0 && $_offset > 0)
        {
            // now search after the end nested { }
            $offset_open = strpos($string, $char, $_offset);
            $offset_close = strpos($string, $charend, $_offset);
            if ($offset_open < $offset_close && !($offset_open === false))
            {
                $_offset = $offset_open+1;
                $_offset_number++;
            }
            else if (!($offset_close === false))
            {
                $_offset = $offset_close+1;
                $_offset_number--;
            }
            else
                $_offset = -1;
        }

        if ($_offset == -1)
            throw new Exception('Protofile failure: ' . $char . ' not nested');

        return array('begin' => $offset_begin, 'end' => $_offset);
    }

    /**
     * Strips the comments out
     * @param String $string the proton file as string
     */
    private function _strip_comments(&$string)
    {
        $string = preg_replace('/\/\/.+/', '', $string);
        // now replace empty lines and whitespaces in front
        $string = preg_replace('/\\r?\\n\s*/', "\n", $string);
    }
	
    /**
     * Parses a service rpc
     * @param String $content - content
     */
	 /*
	 ‘*’ ‘+’ 和 ‘?’
	 “zero or more”相当于{0,}
	 “one or more”相当于{1,}
	 “zero or one.”相当于{0,1}
	 */
    private function _parse_service($content)
    {
		$myarray = $tmp = array();
		$match = preg_match_all('/rpc\s+(\w+)\(\w+\)\s+returns\((\w+)\)\s*;/', $content, $matches, PREG_OFFSET_CAPTURE);
		if ($match)
        {
            for($i=0;$i<count($matches[0]);$i++)
			{
				$tmp['m_name'] = $matches[1][$i][0];
				$tmp['ret_type'] = $matches[2][$i][0];
				$myarray[] = $tmp;
			}
        }
        return $myarray;
    }
	
	/**
     * Creates php class file for the proto file
     *
     * @param String $filename - the filename of the php file
     */
    private function _create_service_file($name)
    {
        $filename = 'pb_service_' . $name . '.php';
		$pb_name = 'pb_proto_' . $name . '.php';
		$string = 
		"define('ROOT_PATH', dirname(__FILE__).'/');\n".
		"define('MSG_PATH', ROOT_PATH.'message/');\n".
		"define('LIB_PATH',ROOT_PATH.'lib/');\n".
		
		"require_once 'tphplib.inc.php';\n".
		"require_once MSG_PATH.'pb_message.php';\n".
		"require_once ROOT_PATH.'$pb_name';\n".
		"require_once LIB_PATH.'L5.php';\n".
		"require_once LIB_PATH.'Log.php';\n".
		"require_once LIB_PATH.'MCMonitor.php';\n".
		"require_once LIB_PATH.'ServerAccessor.php';\n".
		"require_once LIB_PATH.'pb_proto_head.php';\n\n";
        foreach ($this->m_types as $servicefile)
        {
            $servicename = str_replace(".", "_", $servicefile['name']);

            if ($servicefile['type'] == 'service')
            {
                $string .= 'class ' . $servicename  . "\n{\n";
                $string .= "	public static \$server;\n".
                	"	public static function set_server(\$svrinfo)\n".
                	"	{\n".
                	"		$servicename::\$server = \$svrinfo;\n".
                	"	}\n";
                	
					foreach($servicefile['value'] as $index => $service)
					{
						$string .= "	public static function {$servicefile['value'][$index]['m_name']}(\$req)\n".
                		"	{\n".
                		"		\$pbRecv = new {$servicefile['value'][$index]['ret_type']}();\n".
                		"		\n".
                		"		\$bpHead = new Lib_Head();\n".
                		"		\$bpHead->set_ver(1);\n".
                		"		\$bpHead->set_cmd($index);\n".
                		"		\$bpHead->set_seq(0);\n".
                		"		\n".
                		"		if(Lib_ServerAccessor::CallServer($servicename::\$server, \$bpHead, \$req, &\$pbRecv))\n".
                		"			return \$pbRecv;\n".
                		"		return false;\n".
                		"	}\n";
					}
				
            }
			else
			{
				continue;
			}

            // now create the class body with all set and get functions
			$string .= "}\n";
        }
        file_put_contents($filename, '<?php' . "\n" . $string . '?>');
    }	
}
?>
