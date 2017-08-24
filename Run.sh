#!/bin/bash

Blynk_Token=$(<Blynk_Token.txt)
System_Admin=$(<System_Admin.txt)
System_Date=$(date)

less -FX Logo.txt

echo ""
echo "System Admin: $System_Admin"
echo "Start Date: $System_Date"
./CLS_Executable --token="$Blynk_Token"
