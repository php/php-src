<?php

function my_mysqli_data_fields(): array
{
    return [
        'intval' => [
            'type' => '03',
            'charset' => '3f00',
            'length' => '0b000000',
            'flags' => '0110',
            'decimal' => '00',
            'query_data_packet_length' => '080000',
            'query_data_value' => '023134',
            'stmt_data_packet_length' => '0b0000',
            'stmt_data_value' => '0e000000'
        ],
        'fltval' => [
            'type' => '04',
            'charset' => '3f00',
            'length' => '0c000000',
            'flags' => '0110',
            'decimal' => '1f',
            'query_data_packet_length' => '090000',
            'query_data_value' => '03322e33',
            'stmt_data_packet_length' => '0b0000',
            'stmt_data_value' => '33331340',
        ],
        'dblval' => [
            'type' => '05',
            'charset' => '3f00',
            'length' => '16000000',
            'flags' => '0110',
            'decimal' => '1f',
            'query_data_packet_length' => '090000',
            'query_data_value' => '03312e32',
            'stmt_data_packet_length' => '0f0000',
            'stmt_data_value' => '333333333333f33f'
        ],
        'datval' => [
            'type' => '0a',
            'charset' => '3f00',
            'length' => '0a000000',
            'flags' => '8110',
            'decimal' => '00',
            'query_data_packet_length' => '100000',
            'query_data_value' => '0a323031342d31322d3135',
            'stmt_data_packet_length' => '0c0000',
            'stmt_data_value' => '04de070c0f'
        ],
        'timval' => [
            'type' => '0b',
            'charset' => '3f00',
            'length' => '0a000000',
            'flags' => '8110',
            'decimal' => '00',
            'query_data_packet_length' => '0e0000',
            'query_data_value' => '0831333a30303a3032',
            'stmt_data_packet_length' => '100000',
            'stmt_data_value' => '080000000000150801'
        ],
        'dtival' => [
            'type' => '0c',
            'charset' => '3f00',
            'length' => '13000000',
            'flags' => '8110',
            'decimal' => '00',
            'query_data_packet_length' => '190000',
            'query_data_value' => '13323031342d31322d31362031333a30303a3031',
            'stmt_data_packet_length' => '0f0000',
            'stmt_data_value' => '07de070c100d0001'
        ],
        'bitval' => [
            'type' => '10',
            'charset' => '3f00',
            'length' => '40000000',
            'flags' => '2110',
            'decimal' => '00',
            'query_data_packet_length' => '0e0000',
            'query_data_value' => '080808080808080808',
            'stmt_data_packet_length' => '100000',
            'stmt_data_value' => '080808080808080808'
        ],
        'strval' => [
            'type' => 'fd',
            'charset' => 'e000',
            'length' => 'c8000000',
            'flags' => '0110',
            'decimal' => '00',
            'query_data_packet_length' => '0a0000',
            'query_data_value' => '0474657374',
            'stmt_data_packet_length' => '0c0000',
            'stmt_data_value' => '0474657374'
        ],
    ];
}

function my_mysqli_data_field(string $field): array
{
    $fields = my_mysqli_data_fields();
    if (!isset($fields[$field])) {
        throw new Exception("Unknown field $field");
    }
    return $fields[$field];
}



class my_mysqli_fake_packet_item
{
    public function __construct(public string|null $name, public string $value, public bool $is_hex = true)
    {
    }
}

class my_mysqli_fake_packet
{
    private array $data = array();

    public function __get(string $name)
    {
        foreach ($this->data as $item) {
            if ($item->name === $name) {
                return $item->value;
            }
        }
        return null;
    }

    public function __set(string $name, string|my_mysqli_fake_packet_item $value)
    {
        if ($value instanceof my_mysqli_fake_packet_item) {
            if ($value->name === null) {
                $value->name = $name;
            }
        } else {
            $value = new my_mysqli_fake_packet_item($name, $value, true);
        }

        for ($i = 0; $i < count($this->data); $i++) {
            if ($this->data[$i]->name === $name) {
                $this->data[$i] = $value;
                return;
            }
        }

        $this->data[] = $value;
    }

    public function to_bytes(): string
    {
        $bytes = '';
        foreach ($this->data as $item) {
            $bytes .= $item->is_hex ? hex2bin($item->value) : $item->value;
        }
        return $bytes;
    }
}

class my_mysqli_fake_packet_generator
{
    public static function create_packet_item(int|string $value, bool $is_hex = false, string $format = 'v'): my_mysqli_fake_packet_item
    {
        if (is_string($value)) {
            $packed_value = $value;
        } else {
            $packed_value = pack($format, $value);
        }
        return new my_mysqli_fake_packet_item(null, $packed_value, $is_hex);
    }

    public function server_ok(): my_mysqli_fake_packet
    {
        $packet = new my_mysqli_fake_packet();
        $packet->packet_length = "070000";
        $packet->packet_number = "02";
        $packet->header = "00"; // OK
        $packet->affected_rows = "00";
        $packet->last_insert_id = "00";
        $packet->server_status = "0200";
        $packet->warning_count = "0000";
        return $packet;
    }

    public function server_greetings(): my_mysqli_fake_packet
    {
        $packet = new my_mysqli_fake_packet();
        $packet->packet_length = "580000";
        $packet->packet_number = "00";
        $packet->proto_version = "0a";
        $packet->version = self::create_packet_item('5.5.5-10.5.18-MariaDB' . chr(0));
        $packet->thread_id = "03000000";
        $packet->salt = "473e3f6047257c67";
        $packet->filler = "00";
        $packet->server_capabilities = self::create_packet_item(0b1111011111111110);
        $packet->server_character_set = "08";
        $packet->server_status = self::create_packet_item(0b000000000000010);
        $packet->extended_server_capabilities = self::create_packet_item(0b1000000111111111);
        $packet->auth_plugin = "15";
        $packet->unused = "000000000000";
        $packet->mariadb_extended_server_capabilities = self::create_packet_item(0b1111, false, 'V');
        $packet->mariadb_extended_server_capabilities_salt = "6c6b55463f49335f686c643100";
        $packet->mariadb_extended_server_capabilities_auth_plugin = self::create_packet_item('mysql_native_password');

        return $packet;
    }

    public function server_tabular_query_response(): array
    {
        $qr1 = new my_mysqli_fake_packet();
        $qr1->packet_length = "010000";
        $qr1->packet_number = "01";
        $qr1->field_count = "01";

        $qr2 = new my_mysqli_fake_packet();
        $qr2->packet_length = "190000";
        $qr2->packet_number = "02";
        $qr2->catalog_length_plus_name = "0164";
        $qr2->db_length_plus_name = "0164";
        $qr2->table_length_plus_name = "0164";
        $qr2->original_t = "0164";
        $qr2->name_length_plus_name = "0164";
        $qr2->original_n = "0164";
        $qr2->canary = "0c";
        $qr2->charset = "3f00";
        $qr2->length = "0b000000";
        $qr2->type = "03";
        $qr2->flags = "0350";
        $qr2->decimals = "000000";

        $qr3 = new my_mysqli_fake_packet();
        $qr3->full = "05000003fe00002200";

        $qr4 = new my_mysqli_fake_packet();
        $qr4->full = "0400000401350174";

        $qr5 = new my_mysqli_fake_packet();
        $qr5->full = "05000005fe00002200";

        return [$qr1, $qr2, $qr3, $qr4, $qr5];
    }

    public function server_upsert_query_response(): array
    {
        $qr1 = new my_mysqli_fake_packet();
        $qr1->packet_length = "010000";
        $qr1->packet_number = "01";
        $qr1->field_count = "00"; // UPSERT
        $qr1->affected_rows = "00";
        $qr1->affected_rows = "00";
        $qr1->last_insert_id = "00";
        $qr1->server_status = "0000";
        $qr1->warning_count = "0000";
        $qr1->len = "01";
        $qr1->filename = "65";
        $qr1->packet_length = sprintf("%02x0000", strlen($qr1->to_bytes())-4);

        return [$qr1];
    }

    public function server_stmt_prepare_response_start($num_field): my_mysqli_fake_packet
    {
        $pr1 = new my_mysqli_fake_packet();
        $pr1->packet_length = "0c0000";
        $pr1->packet_number = "01";
        $pr1->response_code = '00'; // OK
        $pr1->statement_id = '01000000';
        $pr1->num_fields = $num_field;
        $pr1->num_params = '0000';
        $pr1->filler = '00';
        $pr1->warnings = '0000';

        return $pr1;
    }

    public function server_stmt_prepare_response_end($packer_number): my_mysqli_fake_packet
    {
        $pr3 = new my_mysqli_fake_packet();
        $pr3->packet_length = "050000";
        $pr3->packet_number = $packer_number;
        $pr3->packet_type = 'fe'; // EOF
        $pr3->warnings = '0000';
        $pr3->server_status = '0200';

        return $pr3;
    }

    public function server_stmt_prepare_items_response(): array
    {
        $pr1 = $this->server_stmt_prepare_response_start('0100');

        $pr2 = new my_mysqli_fake_packet();
        $pr2->packet_length = "300000";
        $pr2->packet_number = "02";
        $pr2->catalogue_len = '03';
        $pr2->catalogue = '646566'; // def
        $pr2->db_len = '08';
        $pr2->db = '7068705f74657374'; // php_test
        $pr2->table_len = '05';
        $pr2->table = '6974656d73'; // items
        $pr2->orig_table_len = '05';
        $pr2->orig_table = '6974656d73'; // items
        $pr2->name_len = '04';
        $pr2->name = '6974656d';
        $pr2->orig_name_len = '04';
        $pr2->orig_name = '6974656d';
        $pr2->something = '0c';
        $pr2->charset = 'e000';
        $pr2->length = 'c8000000';
        $pr2->field_type = 'fd'; // FIELD_TYPE_VAR_STRING
        $pr2->flags = '0110';
        $pr2->decimal = '00';
        $pr2->padding = '0000';

        $pr3 = $this->server_stmt_prepare_response_end('03');

        return [$pr1, $pr2, $pr3];
    }

    public function server_stmt_prepare_data_response_field($packet_number, $field_name): my_mysqli_fake_packet
    {
        if (strlen($field_name) != 6) {
            throw new Exception("Invalid field length - only 6 is allowed");
        }

        $field = my_mysqli_data_field($field_name);

        $pr = new my_mysqli_fake_packet();
        $pr->packet_length = "320000";
        $pr->packet_number = $packet_number;
        $pr->catalogue_len = '03';
        $pr->catalogue = bin2hex('def');
        $pr->db_len = '08';
        $pr->db = bin2hex('php_test');
        $pr->table_len = '04';
        $pr->table = bin2hex('data');
        $pr->orig_table_len = '04';
        $pr->orig_table = bin2hex('data');
        $pr->name_len = '06';
        $pr->name = bin2hex($field_name);
        $pr->orig_name_len = '06';
        $pr->orig_name =  bin2hex($field_name);
        $pr->something = '0c';
        $pr->charset = $field['charset'];
        $pr->length = $field['length'];
        $pr->field_type = $field['type'];
        $pr->flags = $field['flags'];
        $pr->decimal = $field['decimal'];
        $pr->padding = '0000';

        return $pr;
    }

    public function server_stmt_prepare_data_response(string $field_name): array
    {
        $pr1 = $this->server_stmt_prepare_response_start('0200');

        $pr2 = $this->server_stmt_prepare_data_response_field('02', 'strval');
        $pr3 = $this->server_stmt_prepare_data_response_field('03', $field_name);

        $pr4 = $this->server_stmt_prepare_response_end('04');

        return [$pr1, $pr2, $pr3, $pr4];
    }

    public function server_stmt_execute_items_response(): array
    {
        $pr1 = new my_mysqli_fake_packet();
        $pr1->packet_length = "010000";
        $pr1->packet_number = "01";
        $pr1->num_fields = '01';

        $pr2 = new my_mysqli_fake_packet();
        $pr2->packet_length = "300000";
        $pr2->packet_number = "02";
        $pr2->catalogue_len = '03';
        $pr2->catalogue = '646566'; // def
        $pr2->db_len = '08';
        $pr2->db = '7068705f74657374'; // php_test
        $pr2->table_len = '05';
        $pr2->table = '6974656d73'; // items
        $pr2->orig_table_len = '05';
        $pr2->orig_table = '6974656d73'; // items
        $pr2->name_len = '04';
        $pr2->name = '6974656d';
        $pr2->orig_name_len = '04';
        $pr2->orig_name = '6974656d';
        $pr2->something = '0c';
        $pr2->charset = 'e000';
        $pr2->length = 'c8000000';
        $pr2->field_type = 'fd'; // FIELD_TYPE_VAR_STRING
        $pr2->flags = '0110';
        $pr2->decimal = '00';
        $pr2->padding = '0000';

        $pr3 = new my_mysqli_fake_packet();
        $pr3->packet_length = "050000";
        $pr3->packet_number = "03";
        $pr3->packet_type = 'fe'; // EOF
        $pr3->warnings = '0000';
        $pr3->server_status = '2200';

        $pr4 = new my_mysqli_fake_packet();
        $pr4->packet_length = "070000";
        $pr4->packet_number = "04";
        $pr4->packet_type = '00'; // OK
        $pr4->affected_rows = '00';
        $pr4->row_data_len = '04';
        $pr4->row_data = '74657374'; // item

        $pr5 = new my_mysqli_fake_packet();
        $pr5->full = '05000005fe00002200';

        return [$pr1, $pr2, $pr3, $pr4, $pr5];
    }

    private function server_execute_data_response_start(string $field_name): array
    {
        $pr1 = new my_mysqli_fake_packet();
        $pr1->packet_length = "010000";
        $pr1->packet_number = "01";
        $pr1->num_fields = '02';

        $pr2 = new my_mysqli_fake_packet();
        $pr2->packet_length = "320000";
        $pr2->packet_number = "02";
        $pr2->catalogue_len = '03';
        $pr2->catalogue = '646566'; // def
        $pr2->db_len = '08';
        $pr2->db = '7068705f74657374'; // php_test
        $pr2->table_len = '04';
        $pr2->table = bin2hex('data');
        $pr2->orig_table_len = '04';
        $pr2->orig_table = bin2hex('data');
        $pr2->name_len = '06';
        $pr2->name = bin2hex('strval');
        $pr2->orig_name_len = '06';
        $pr2->orig_name =  bin2hex('strval');
        $pr2->something = '0c';
        $pr2->charset = 'e000';
        $pr2->length = 'c8000000';
        $pr2->field_type = 'fd'; // FIELD_TYPE_VAR_STRING
        $pr2->flags = '0110';
        $pr2->decimal = '00';
        $pr2->padding = '0000';

        $field = my_mysqli_data_field($field_name);

        $pr3 = new my_mysqli_fake_packet();
        $pr3->packet_length = "320000";
        $pr3->packet_number = "03";
        $pr3->catalogue_len = '03';
        $pr3->catalogue = '646566'; // def
        $pr3->db_len = '08';
        $pr3->db = '7068705f74657374'; // php_test
        $pr3->table_len = '04';
        $pr3->table = bin2hex('data');
        $pr3->orig_table_len = '04';
        $pr3->orig_table = bin2hex('data');
        $pr3->name_len = '06';
        $pr3->name = bin2hex($field_name);
        $pr3->orig_name_len = '06';
        $pr3->orig_name =  bin2hex($field_name);
        $pr3->something = '0c';
        $pr3->charset = $field['charset'];
        $pr3->length = $field['length'];
        $pr3->field_type = $field['type'];
        $pr3->flags = $field['flags'];
        $pr3->decimal = $field['decimal'];
        $pr3->padding = '0000';

        $pr4 = new my_mysqli_fake_packet();
        $pr4->packet_length = "050000";
        $pr4->packet_number = "04";
        $pr4->packet_type = 'fe'; // EOF
        $pr4->warnings = '0000';
        $pr4->server_status = '2200';

        return [$field, $pr1, $pr2, $pr3, $pr4];
    }

    private function server_execute_data_response_end(): my_mysqli_fake_packet
    {
        $pr6 = new my_mysqli_fake_packet();
        $pr6->packet_length = '050000';
        $pr6->packet_number = "06";
        $pr6->packet_type = 'fe'; // EOF
        $pr6->warnings = '0000';
        $pr6->server_status = '2200';

        return $pr6;
    }

    public function server_stmt_execute_data_response(string $field_name): array
    {
        [$field, $pr1, $pr2, $pr3, $pr4] = $this->server_execute_data_response_start($field_name);

        $pr5 = new my_mysqli_fake_packet();
        $pr5->packet_length = $field['stmt_data_packet_length'];
        $pr5->packet_number = "05";
        $pr5->packet_type = '00'; // OK
        $pr5->affected_rows = '00';
        $pr5->row_field1_len = '04';
        $pr5->row_field1_data = '74657374'; // test
        $pr5->row_field2 = $field['stmt_data_value'];

        return [$pr1, $pr2, $pr3, $pr4, $pr5, $this->server_execute_data_response_end()];
    }

    public function server_query_execute_data_response(string $field_name): array
    {
        [$field, $pr1, $pr2, $pr3, $pr4] = $this->server_execute_data_response_start($field_name);

        $pr5 = new my_mysqli_fake_packet();
        $pr5->packet_length = $field['query_data_packet_length'];
        $pr5->packet_number = "05";
        $pr5->row_field1_len = '04';
        $pr5->row_field1_data = '74657374'; // test
        $pr5->row_field2 = $field['query_data_value'];

        return [$pr1, $pr2, $pr3, $pr4, $pr5, $this->server_execute_data_response_end()];
    }
}

class my_mysqli_fake_server_conn
{
    private $conn;
    public $packet_generator;

    public function __construct($socket)
    {
        $this->packet_generator = new my_mysqli_fake_packet_generator();
        $this->conn = stream_socket_accept($socket);
        if ($this->conn) {
            fprintf(STDERR, "[*] Connection established\n");
        } else {
            fprintf(STDERR, "[*] Failed to establish connection\n");
        }
    }

    public function packets_to_bytes(array $packets): string
    {
        return implode('', array_map(fn($s) => $s->to_bytes(), $packets));
    }

    public function send($payload, $message = null): void
    {
        if ($message) {
            fprintf(STDERR, "[*] Sending - %s: %s\n", $message, bin2hex($payload));
        }
        fwrite($this->conn, $payload);
    }

    public function read($bytes_len = 1024)
    {
        // wait 10ms to fill the buffer
        usleep(10000);
        $data = fread($this->conn, $bytes_len);
        if ($data) {
            fprintf(STDERR, "[*] Received: %s\n", bin2hex($data));
        }
    }

    public function close()
    {
        fclose($this->conn);
    }

    public function send_server_greetings()
    {
        $this->send($this->packet_generator->server_greetings()->to_bytes(), "Server Greeting");
    }

    public function send_server_ok()
    {
        $this->send($this->packet_generator->server_ok()->to_bytes(), "Server OK");
    }

    public function send_server_tabular_query_response(): void
    {
        $packets = $this->packet_generator->server_tabular_query_response();
        $this->send($this->packets_to_bytes($packets), "Tabular response");
    }

    public function send_server_stmt_prepare_items_response(): void
    {
        $packets = $this->packet_generator->server_stmt_prepare_items_response();
        $this->send($this->packets_to_bytes($packets), "Stmt prepare items");
    }


    public function send_server_stmt_prepare_data_response(string $field_name): void
    {
        $packets = $this->packet_generator->server_stmt_prepare_data_response($field_name);
        $this->send($this->packets_to_bytes($packets), "Stmt prepare data $field_name");
    }

    public function send_server_stmt_execute_items_response(): void
    {
        $packets = $this->packet_generator->server_stmt_execute_items_response();
        $this->send($this->packets_to_bytes($packets), "Stmt execute items");
    }

    public function send_server_stmt_execute_data_response(string $field_name): void
    {
        $packets = $this->packet_generator->server_stmt_execute_data_response($field_name);
        $this->send($this->packets_to_bytes($packets), "Stmt execute data $field_name");
    }

    public function send_server_query_execute_data_response(string $field_name): void
    {
        $packets = $this->packet_generator->server_query_execute_data_response($field_name);
        $this->send($this->packets_to_bytes($packets), "Query execute data $field_name");
    }
}

class my_mysqli_fake_server_process
{
    public function __construct(private $process, private array $pipes) {}

    public function terminate(bool $wait = false)
    {
        if ($wait) {
            $this->wait();
        }
        proc_terminate($this->process);
    }

    public function wait()
    {
        echo fgets($this->pipes[1]);
    }
}

function my_mysqli_test_tabular_response_def_over_read(my_mysqli_fake_server_conn $conn): void
{
    $rh = $conn->packet_generator->server_tabular_query_response();

    // Length of the packet is modified to include the next added data
    $rh[1]->packet_length = "1e0000";

    // We add a length field encoded on 4 bytes which evaluates to 65536. If the process crashes because
    // the heap has been overread, lower this value.
    $rh[1]->extra_def_size = "fd000001";  # 65536

    // Filler
    $rh[1]->extra_def_data = "aa";

    $trrh = $conn->packets_to_bytes($rh);

    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $conn->send($trrh, "Malicious Tabular Response [Extract heap through buffer over-read]");
    $conn->read(65536);
}

function my_mysqli_test_upsert_response_filename_over_read(my_mysqli_fake_server_conn $conn): void
{
    $rh = $conn->packet_generator->server_upsert_query_response();

    // Set extra length to overread
    $rh[0]->len = "fa";

    $trrh = $conn->packets_to_bytes($rh);

    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $conn->send($trrh, "Malicious Tabular Response [Extract heap through buffer over-read]");
    $conn->read(65536);
}

function my_mysqli_test_auth_response_message_over_read(my_mysqli_fake_server_conn $conn): void
{
    $p = $conn->packet_generator->server_ok();
    $p->packet_length = "090000";
    $p->message_len = "fcff";

    $conn->send_server_greetings();
    $conn->read();
    $conn->send($p->to_bytes(), "Malicious OK Auth Response [Extract heap through buffer over-read]");
    $conn->read();
}

function my_mysqli_test_stmt_response_row_over_read_string(my_mysqli_fake_server_conn $conn): void
{
    $rh = $conn->packet_generator->server_stmt_execute_items_response();

    // Set extra length to overread
    $rh[3]->row_data_len = "fa";

    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $conn->send_server_stmt_prepare_items_response();
    $conn->read();
    $conn->send($conn->packets_to_bytes($rh), "Malicious Stmt Response for items [Extract heap through buffer over-read]");
    $conn->read(65536);
}

function my_mysqli_test_stmt_response_row_over_read_two_fields(
    my_mysqli_fake_server_conn $conn,
    string $field_name,
    string $row_field1_len = '06'
): void {
    $rh = $conn->packet_generator->server_stmt_execute_data_response($field_name);

    // Set extra length to overread by two bytes
    $rh[4]->row_field1_len = $row_field1_len;

    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $conn->send_server_stmt_prepare_data_response($field_name);
    $conn->read();
    $conn->send(
        $conn->packets_to_bytes($rh),
        "Malicious Stmt Response for data $field_name [Extract heap through buffer over-read]"
    );
    $conn->read(65536);
}

function my_mysqli_test_stmt_response_row_over_read_int(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'intval');
}

function my_mysqli_test_stmt_response_row_over_read_float(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'fltval');
}

function my_mysqli_test_stmt_response_row_over_read_double(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'dblval');
}

function my_mysqli_test_stmt_response_row_over_read_date(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'datval');
}

function my_mysqli_test_stmt_response_row_over_read_time(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'timval', '0c');
}

function my_mysqli_test_stmt_response_row_over_read_datetime(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'dtival');
}

function my_mysqli_test_stmt_response_row_no_space(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'strval', '09');
}

function my_mysqli_test_stmt_response_row_over_read_bit(my_mysqli_fake_server_conn $conn): void
{
    my_mysqli_test_stmt_response_row_over_read_two_fields($conn, 'bitval');
}

function my_mysqli_test_stmt_response_row_read_two_fields(my_mysqli_fake_server_conn $conn): void
{
    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $field_names = array_keys(my_mysqli_data_fields());
    foreach ($field_names as $field_name) {
        $conn->send_server_stmt_prepare_data_response($field_name);
        $conn->read(65536);
        $conn->send_server_stmt_execute_data_response($field_name);
        $conn->read(65536);
    }
}

function my_mysqli_test_query_response_row_length_overflow(my_mysqli_fake_server_conn $conn): void
{
    $rh = $conn->packet_generator->server_query_execute_data_response('strval');

    // Set extra length to overread by two bytes
    $rh[4]->row_field2 = 'fefefefefe';

    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $conn->send($conn->packets_to_bytes($rh), "Malicious Query Response for data strval field [length overflow]");
    $conn->read(65536);
}

function my_mysqli_test_query_response_row_read_two_fields(my_mysqli_fake_server_conn $conn): void
{
    $conn->send_server_greetings();
    $conn->read();
    $conn->send_server_ok();
    $conn->read();
    $field_names = array_keys(my_mysqli_data_fields());
    foreach ($field_names as $field_name) {
        $conn->send_server_query_execute_data_response($field_name);
        $conn->read();
    }
}

function run_fake_server(string $test_function, $port = 33305): void
{
    $address = '127.0.0.1';

    $socket = @stream_socket_server("tcp://$address:$port", $errno, $errstr);
    if (!$socket) {
        die("Failed to create socket: $errstr ($errno)\n");
    }
    echo "[*] Server started\n";

    try {
        $conn = new my_mysqli_fake_server_conn($socket);
        $test_function_name = 'my_mysqli_test_' . $test_function;
        call_user_func($test_function_name, $conn);
        $conn->close();
    } catch (Exception $e) {
        fprintf(STDERR, "[!] Exception: " . $e->getMessage() . "\n");
    }

    fclose($socket);

    echo "[*] Server finished\n";
}


function run_fake_server_in_background($test_function, $port = 33305): my_mysqli_fake_server_process
{
    $command = [PHP_BINARY, '-n', __FILE__, 'mysqli_fake_server', $test_function, $port];

    $descriptorspec = array(
        0 => array("pipe", "r"),
        1 => array("pipe", "w"),
        2 => STDERR,
    );

    $process = proc_open($command, $descriptorspec, $pipes);

    if (is_resource($process)) {
        return new my_mysqli_fake_server_process($process, $pipes);
    } else {
        throw new Exception("Failed to start server process");
    }
}

if (isset($argv) && $argc > 2 && $argv[1] == 'mysqli_fake_server') {
    run_fake_server($argv[2], $argv[3] ?? '33305');
}
