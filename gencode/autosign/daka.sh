#!/bin/bash
set -x -e
token="hmp_29c5d4bb322ef22bc1a942d4800c2b2e22d6f6a8c62b446b3cd643ab2d8ddd93"
robotID="63eaf1a9a069fe2a08e80cf6"
robotName="牛气貂"
url="https://api.hamibot.com/v1/devscripts/"
upTaskID="63eaf63ea069fe2a08e89592"
downTaskID="63eb0d4aa069fe2a08eb4728"

dakaTask ()
{
    curl -H "Authorization: ${token}" ${url}/$1/run -X POST -H "Content-Type: application/json" -d "{\"robots\": [{\"_id\": \"${robotID}\", \"name\": \"${robotName}\"}]}"
}

updaka ()
{
    # 移动运维
    dakaTask ${upTaskID}
}

downdaka ()
{
    # 移动运维
    dakaTask ${downTaskID}
}

case "$1" in
    up) 
        updaka
    ;;
    down)
        downdaka
    ;;
    *) echo "input error"
    ;;
esac
