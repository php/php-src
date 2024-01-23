<?php

function searchEmailByToAddress(string $to): ?array
{
    $query = urlencode("to:{$to}");
    $res = file_get_contents("http://localhost:8025/api/v1/search?query=$query");
    $list = json_decode($res, true);

    $id = $list['messages'][0]['ID'] ?? null;
    if (!$id) {
        return null;
    }

    $res = file_get_contents("http://localhost:8025/api/v1/message/{$id}");
    return json_decode($res, true);
}

function getHeaders($res): array
{
    $headerRes = file_get_contents("http://localhost:8025/api/v1/message/{$res['ID']}/headers");
    return json_decode($headerRes, true);
}

function mailCheckResponse($res, string $from, string $to, string $subject, string $message): bool
{
    return $res &&
        ($res['From']['Address'] ?? null) === $from &&
        ($res['To'][0]['Address'] ?? null) === $to &&
        ($res['Subject'] ?? null) === $subject &&
        trim($res['Text'] ?? '') === trim($message);
}

function getCcAddresses($res): array
{
    return array_column($res['Cc'], 'Address');
}

function getBccAddresses($res): array
{
    return array_column($res['Bcc'], 'Address');
}

function deleteEmailByToAddress(string $to)
{
    $query = urlencode("to:{$to}");
    file_get_contents("http://localhost:8025/api/v1/search?query={$query}", false, stream_context_create([
        'http' => [
            'method' => 'DELETE',
        ],
    ]));
}
?>
