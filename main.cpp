#include <dpp/dpp.h>
#include <thread>
#include <stdlib.h>
#include <fstream>

int main()
{
    dpp::cluster bot("token");

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot](const dpp::slashcommand_t & event) 
    {
        /* Check which command they ran */
        if (event.command.get_command_name() == "host") 
        {
            /* Fetch a parameter value from the command parameters */
            std::string url = std::get<std::string>(event.get_parameter("url"));

            /* Reply to the command. There is an overloaded version of this
            * call that accepts a dpp::message so you can send embeds.
            */
            event.reply(std::string("Cloning repo..."));
            url.append(" build");
            system(std::string("git clone ").append(url).c_str());
            event.edit_response("Building... (can take >5m)");
            system("cd build && ./gradlew applyPatches && ./gradlew createReobfBundlerJar");
            event.edit_response("Starting server...");
            system("mkdir sv");
            event.edit_response("Server started on 77.230.98.46:25565");
            for (const auto & entry : std::filesystem::directory_iterator("build/build/libs"))
                system(std::string("cd sv && java -Xmx3G -jar ../").append(entry.path()).c_str());
            }
            
    });

   bot.on_ready([&bot](const dpp::ready_t & event) 
   {
        if (dpp::run_once<struct register_bot_commands>()) 
        {
            /* Create a new global command on ready event */
            dpp::slashcommand newcommand("host", "Host an mc server with a simple command and a git url!", bot.me.id);
            newcommand.add_option(
                dpp::command_option(dpp::co_string, "url", "The git url of the server you want to run (gradle only for now)", true)
            );
 
            /* Register the command */
            bot.global_command_create(newcommand);
        }
        std::cout << "bot ready!\n";
    });

    bot.start(dpp::st_wait);
}
