#!/bin/bash

#Main Install Function
function Start_Install()
{
 echo "Continuing installation."
 echo "Please enter the system admin's name..."
 read System_Admin

 case "$System_Admin" in
   *[!\ ]*)  echo "$System_Admin" > System_Admin.txt;;
   *) echo "Not Assigned" > System_Admin.txt
      echo "Note: A system admin has not been set for this CLS."
      echo "This can be modified in one of two ways:"
      echo "  > Re-run the Install script or,"
      echo "  > Modify the name directly in the System Admin cache file"
      echo "This CLS implementation can still run without this property set."
      echo "";;
 esac

 echo "Please enter the Blynk project token for this CLS..."
 read Blynk_Token

 case "$Blynk_Token" in
   *[!\ ]*) echo "$Blynk_Token" > Blynk_Token.txt
            chmod u=rx build.sh
            chmod u=r Logo.txt
            chmod u=rwx CLS_Executable
            chmod u=rx  Run.sh
            echo ""
            echo "Executing build script..."
            echo ""
            ./build.sh
            echo ""
            echo "Installation complete.";;
   *) echo "Not Assigned" > Blynk_Token.txt
      echo "Note: A Blynk_Token has not been entered"
      echo "Please perform one of the two actions:"
      echo "  > Re-run the Install script or,"
      echo "  > Enter the token into the Blynk_Token cache file and manually run the build script"
      echo "This CLS implementation cannot run without this property set."
      echo ""
      echo "Process halted"
      echo ""
      chmod u=r Run.sh;;
 esac
}

#Main Executable Function
echo "All previous data will be overwritten."
echo "Are you sure you want to proceed? [y/n]"
read Proceed_Answer

case "$Proceed_Answer" in
   [yY]) Start_Install;;
   *) echo "Process Halted";;
esac

exit 1

