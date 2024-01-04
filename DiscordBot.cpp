#include "DiscordBot.hpp"
#include "default_functions.hpp"
#include <dpp/dpp.h>

#include <iostream>
#include <queue>
#include <mutex>

using namespace std;
using namespace TXT_Style;

queue<pair<uint64_t, string>> message_queue;
mutex queue_mutex;

//funcao que adiciona a mensagem de envio na fila para ser postada no discord.
void Discord_Tools::discord_enqueue_message(uint64_t channel_id, const string& message) {

    Exlog(AZUL + __func__, VERDE + "Mensagem sendo adicionada a fila (queue)");

    //adiciona a mensagem recebida a fila de envio
    message_queue.push({ channel_id, message });

    Exlog(AZUL + __func__, VERDE + "Mensagem enviada com sucesso a fila." + " | channel_id: " + to_string(channel_id) + " | message: " + message);
}

void Discord_Tools::discord_messenger() {

    Exlog(AZUL + __func__, VERDE + "Started!");

    dpp::cluster bot(DiscordBotToken);
    
    Exlog(AZUL + __func__, VERDE + "Token: " + CIANO + DiscordBotToken.get<string>());
     
    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&](const dpp::ready_t& event) {

        while (true) {

            //quando entrar na funcao, garantir que sera executada apenas uma vez por vez.
            queue_mutex.lock();

            if (!message_queue.empty()) {

                //enquanto a fila tiver conteudo na frente
                for (queue<pair<uint64_t, string>> temp_queue = message_queue; !temp_queue.empty(); temp_queue.pop()) {

                    pair<uint64_t, string> message = temp_queue.front();

                    Exlog(AZUL + __func__, VERDE + "ID: " + CIANO + to_string(message.first) + VERDE + ", Mensagem: " + CIANO + message.second);

                }

                //busca no pair o id do canal, e a mensangem (como um push_front)
                auto [channel_id, message] = message_queue.front();

                //deleta a mensagem, pois ela sera enviada e nao precisaremos mais.
                message_queue.pop();

                //desbloqueia a funcao apos deletar a mensagem.
                queue_mutex.unlock();

                //envia de fato a mensagem para o canal do discord.
                dpp::message m;
                m.set_content(message);
                m.set_channel_id(channel_id);
                bot.message_create(m);

                Exlog(AZUL + __func__, VERDE + "Finalizado processo de envio da mensagem.");

            }
            else {
                queue_mutex.unlock();
            }

            //verifica a fila de mensagens a cada 2 segundos buscando as mensagens e canais para envio.
            this_thread::sleep_for(chrono::milliseconds(2000));

            //Exlog(AZUL + __func__, VERDE + "passed sleep_for discord messenger");
            
        }

        Exlog(AZUL + __func__, VERDE + "On Loop!");

    });

    Exlog(AZUL + __func__, VERDE + "--> Discord Messenger Inicializado.");


    Directories::DiscordToolsRoutesOnEnd = true;

    //inicia o bot.
    bot.start(dpp::st_wait);
}