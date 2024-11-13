<?php

class MailConnecter
{
    private $fp = null;
    private static $instance;
    private static $init = false;

    private const HOST = 'localhost';
    private const PORT = 143;
    private const TIMEOUT = 10;

    private function __construct()
    {
        if (($this->fp = @fsockopen(self::HOST, self::PORT)) === false) {
            die('cannot open imap socket');
        }
    }

    public static function getConnection(): self
    {
        if (!static::$init) {
            static::$instance = new self();
        }

        return static::$instance;
    }

    public function disconnect(): void
    {
        fclose($this->fp);
    }

    public function fail(string $message): void
    {
        $this->disconnect();
        die($message);
    }

    public function send(string $tag, string $command): void
    {
        fputs($this->fp, "{$tag} {$command}\r\n");
    }

    public function isSuccess(string $tag): bool
    {
        $start = time();
        while (!feof($this->fp)) {
            $line = fgets($this->fp);
            if (!$line) {
                return false;
            }
            if (str_contains($line, $tag)) {
                if (preg_match('/(NO|BAD|failed|Failure)/i', $line)) {
                    return false;
                }
                return true;
            }
            if (time() - $start > self::TIMEOUT) {
                $this->fail("{$tag} timeout");
            }
        }
        return false;
    }

    public function getResponse(string $tag, bool $returnArray = false): string|array
    {
        $start = time();
        $output = $returnArray ? [] : '';
        while (!feof($this->fp)) {
            $line = fgets($this->fp);
            if (!$line) {
                $this->fail("{$tag} failed");
            }
            if ($returnArray) {
                $output[] = $line;
            } else {
                $output .= $line;
            }
            if (str_contains($line, $tag)) {
                if (preg_match('/(NO|BAD|failed|Failure)/i', $line)) {
                    $this->fail("{$tag} failed");
                }
                return $output;
            }
            if (time() - $start > self::TIMEOUT) {
                $this->fail("{$tag} timeout");
            }
        }
        $this->fail("{$tag} failed");
    }
}

class MailBox
{
    private MailConnecter $mailConnecter;
    private const PASSWORD = 'p4ssw0rd';
    public const USERS = [
        'webmaster@example.com',
        'info@example.com',
        'admin@example.com',
        'foo@example.com',
    ];

    private const LOGIN = 'A00001';
    private const LOGOUT = 'A00002';
    private const SELECT_MAILBOX = 'A00003';
    private const SEARCH = 'A00004';
    private const FETCH_HEADERS = 'A00005';
    private const FETCH_BODY = 'A00006';
    private const DELETE = 'A00007';
    private const EXPUNGE = 'A00008';

    private function __construct()
    {
        $this->mailConnecter = MailConnecter::getConnection();
    }

    public static function login(string $user): self
    {
        $self = new self();
        $self->mailConnecter->send(self::LOGIN, 'LOGIN '.$user.' '.self::PASSWORD);
        if (!$self->mailConnecter->isSuccess(self::LOGIN)) {
            $self->mailConnecter->fail('login failed');
        }
        return $self;
    }

    public function logout(): void
    {
        $this->mailConnecter->send(self::LOGOUT, 'LOGOUT');
        if (!$this->mailConnecter->isSuccess(self::LOGOUT)) {
            $this->mailConnecter->fail('logout failed');
        }
    }

    private function getUidsBySubject(string $subject): array
    {
        $this->mailConnecter->send(self::SELECT_MAILBOX, 'SELECT "INBOX"');
        if (!$this->mailConnecter->isSuccess(self::SELECT_MAILBOX)) {
            $this->mailConnecter->fail('select mailbox failed');
        }

        $this->mailConnecter->send(self::SEARCH, "UID SEARCH SUBJECT \"{$subject}\"");
        $res = $this->mailConnecter->getResponse(self::SEARCH);
        preg_match('/SEARCH ([0-9 ]+)/is', $res, $matches);
        return isset($matches[1]) ? explode(' ', trim($matches[1])) : [];
    }

    public function getMailsBySubject(string $subject): MailCollection
    {
        return new MailCollection(array_map(
            fn($uid) => $this->getHeaders($uid) + ['Body' => $this->getBody($uid)],
            $this->getUidsBySubject($subject),
        ));
    }

    private function getHeaders(int $uid): array
    {
        $this->mailConnecter->send(self::FETCH_HEADERS, "UID FETCH {$uid} (BODY[HEADER])");
        $res = $this->mailConnecter->getResponse(self::FETCH_HEADERS, true);

        $headers = [];
        foreach ($res as $line) {
            $line = trim($line);
            if (!$line) {
                continue;
            }
            $items = explode(':', $line);
            preg_match('/^(.+?):(.+?)$/', $line, $matches);
            $key = trim($matches[1] ?? '');
            $val = trim($matches[2] ?? '');
            if (!$key || !$val || $val === self::FETCH_HEADERS.' OK UID completed' || $val === ')') {
                continue;
            }

            $headers[$key] = $val;
        }
        return $headers;
    }

    private function getBody(int $uid): string
    {
        $this->mailConnecter->send(self::FETCH_BODY, "UID FETCH {$uid} (BODY[TEXT])");
        $body = $this->mailConnecter->getResponse(self::FETCH_BODY, true);
        $count = count($body);
        if ($count <= 3) {
            return '';
        }

        return implode('', array_slice($body, 1, $count - 3));
    }

    public function deleteMailsBySubject(string $subject): void
    {
        array_map(
            fn($uid) => $this->mailConnecter->send(self::DELETE, "UID STORE {$uid} +FLAGS (\\Deleted)"),
            $this->getUidsBySubject($subject),
        );
        $this->mailConnecter->send(self::EXPUNGE, 'EXPUNGE');
    }
}

class MailCollection
{
    public function __construct(private ?array $mailData) {}

    public function isAsExpected(string $from, string $to, string $subject, string $message): bool
    {
        $result = true;

        if (!$this->mailData) {
            $result = false;
            echo "Email data does not exist.\n";
        }
        if (!$this->count() > 1) {
            $result = false;
            echo "Multiple email data exist.\n";
        }
        if ($this->getHeader('From', true) !== $from) {
            $result = false;
            echo "from does not match.\n";
        }
        if ($this->getHeader('To', true) !== $to) {
            $result = false;
            echo "to does not match.\n";
        }
        if ($this->getHeader('Subject', true) !== $subject) {
            $result = false;
            echo "subject does not match.\n";
        }
        if (trim($this->getBody()) !== trim($message)) {
            $result = false;
            echo "body does not match.\n";
        }

        return $result;
    }

    public function count(): int
    {
        return count($this->mailData);
    }

    public function getHeader(string $field, bool $ignoreCases = false, int $offset = 0)
    {
        if ($ignoreCases) {
            $mail =  array_change_key_case($this->mailData[$offset] ?? []);
            $field = strtolower($field);
        } else {
            $mail = $this->mailData[$offset] ?? [];
        }

        return $mail[$field] ?? null;
    }

    public function getBody(int $offset = 0): ?string
    {
        return $this->mailData[$offset]['Body'] ?? null;
    }
}
