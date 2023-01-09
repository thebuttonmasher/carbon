        ==============================================================================================

                     ('-.     _  .-') .-. .-')                    .-') _    ('-.  ) (`-.        ('-.   
                    ( OO ).-.( \( -O )\  ( OO )                  ( OO ) ) _(  OO)  ( OO ).    _(  OO)  
           .-----.  / . --. / ,------. ;-----.\  .-'),-----. ,--./ ,--,' (,------.(_/.  \_)-.(,------. 
          '  .--./  | \-.  \  |   /`. '| .-.  | ( OO'  .-.  '|   \ |  |\  |  .---' \  `.'  /  |  .---' 
          |  |('-..-'-'  |  | |  /  | || '-' /_)/   |  | |  ||    \|  | ) |  |      \     /\  |  |     
         /_) |OO  )\| |_.'  | |  |_.' || .-. `. \_) |  |\|  ||  .     |/ (|  '--.    \   \ | (|  '--.  
         ||  |`-'|  |  .-.  | |  .  '.'| |  \  |  \ |  | |  ||  |\    |   |  .--'   .'    \_) |  .--'  
        (_'  '--'\  |  | |  | |  |\  \ | '--'  /   `'  '-'  '|  | \   |.-.|  `---. /  .'.  \  |  `---. 
           `-----'  `--' `--' `--' '--'`------'      `-----' `--'  `--'`-'`------''--'   '--' `------' 

        ==============================================================================================

    version: 1.0

   The tool works like this:
   First, our victim opens loader.exe as administrator.
   Loader.exe then drops our payload, carbon.exe, and creates some registry keys and values,
   so that it runs together with CompatTelRunner.exe once per day.

   Carbon.exe creates a server on localhost, port 27017, and is ready to receive commands!
   
   For ease of use there is also a client provided for sending commands.

        ==============================================================================================

   Compilation instructions:
   

   Compiled using MSVC 2022.
   Tested on windows 10 22H2 Build 19045.

   1. Build Main.cpp using MSVC
   2. Change the path in Carbon.RC to match the carbon.exe you just built
   3. Build Loader.cpp using MSVC
   4. (Optional but highly reccomended) Build client.cpp using MSVC

   it's important to build carbon before you build the loader because carbon is a resource
   inside the loader!

        ==============================================================================================

   Client usage:
   Client.exe server-name port-number command optional=command-args
   Command list:
   "1" : get contents of file, path must be supplied as "command-args"! 

        ==============================================================================================

   Notes:
   In retrospect I have made some design mistakes that I will avoid in the future.
   1. I should have created a main Carbon class for easier resource sharing, so functions like
      do_command wouldn't need to accept a socket as a parameter.
   2. Winsock is not very fun to use, in hindsight a 3rd party library would help here.
   3. Logging and error handling should be the first thing to think about when writing code,
      and I've sinned a bit here by giving it less importance than I should have.



   
