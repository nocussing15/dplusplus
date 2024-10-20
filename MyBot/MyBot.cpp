#include "MyBot.h"
#include <dpp/dpp.h>

/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the 
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */
const std::string    BOT_TOKEN    = "add your token here";

int main()
{
    /* Create bot cluster */
    dpp::cluster bot(BOT_TOKEN);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Register slash command here in on_ready */
    bot.on_ready([&bot](const dpp::ready_t& event) {
        /* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
        if (dpp::run_once<struct register_bot_commands>()) {
            std::vector<dpp::slashcommand> commands{
                { "ping", "Ping pong!", bot.me.id },
                { "pm", "Send a private message", bot.me.id}
            };



            bot.global_bulk_command_create(commands);

        }
        });

    /* Handle slash command with the most recent addition to D++ features, coroutines! */
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) -> dpp::task<void> {
        if (event.command.get_command_name() == "ping") {
            co_await event.co_reply("Pong!");
        }

        if (event.command.get_command_name() == "pm") {
            dpp::snowflake user;

            //if there is no specified user, we set the "user" variable to the command author
            if (event.get_parameter("user").index() == 0) {
                user = event.command.get_issuing_user().id;
            }
            else { // otherwise, we set user to the specified user
                user = std::get<dpp::snowflake>(event.get_parameter("user"));
            }

            bot.direct_message_create(user, dpp::message("Here's a private message!"), [event, user](const dpp::confirmation_callback_t& callback) {
                // if the call back errors, we want to send a message telling the author that something went wrong
                if (callback.is_error()) {
                    if (user == event.command.get_issuing_user().id) {
                        event.reply(dpp::message("I couldnt send you a message.").set_flags(dpp::m_ephemeral));
                    }
                    else {
                        event.reply(dpp::message("I couldn't send a message to that user. Please check that is a valid user!").set_flags(dpp::m_ephemeral));
                    }
                    return;
                }
                //same in case sent to specifed user

                if (user == event.command.get_issuing_user().id) {
                    event.reply(dpp::message("I've snet you a private message.").set_flags(dpp::m_ephemeral));
                }
                else {
                    event.reply(dpp::message("i've sent a message to the user.").set_flags(dpp::m_ephemeral));
                }
                });
        }

        co_return;
        });

	/* Start the bot */
	bot.start(dpp::st_wait);

	return 0;
}
