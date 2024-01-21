<?php

function searchEmailByToAddress (string $to)
{
    // Search the mail by $to
    $c = curl_init();
    curl_setopt($c, CURLOPT_URL, 'http://localhost:8025/api/v2/search?kind=to&query='.$to);
    curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
    $res = curl_exec($c);
    curl_close($c);

    return json_decode($res, true);
}

function getHeaders ($response)
{
    return $response['items'][0]['Content']['Headers'] ?? [];
}

function getFromAddress ($response)
{
    $data = $response['items'][0]['From'] ?? [];
    return isset($data['Mailbox'], $data['Domain'])
        ? $data['Mailbox'].'@'.$data['Domain']
        : null;
}

function getToAddresses ($response)
{
    $data = $response['items'][0]['To'] ?? [];

    $addresses = [];
    foreach ($data as $d) {
        $addresses[] = $d['Mailbox'].'@'.$d['Domain'];
    }
    return $addresses;
}

function getRecipientAddresses ($response)
{
    return $response['items'][0]['Raw']['To'] ?? [];
}

function getSubject ($response)
{
    return $response['items'][0]['Content']['Headers']['Subject'][0] ?? null;
}

function getBody ($response)
{
    return $response['items'][0]['Content']['Body'] ?? null;
}

function deleteEmail ($response)
{
    $id = $response['items'][0]['ID'] ?? null;
    if ($id) {
        $c = curl_init();
        curl_setopt($c, CURLOPT_URL, 'http://localhost:8025/api/v2/messages/'.$id);
        curl_setopt($c, CURLOPT_CUSTOMREQUEST, 'DELETE');
        curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
        curl_exec($c);
        curl_close($c);
    }
}
?>
