//coletando informações após o usuario apertar o botão enviar.
document.addEventListener("DOMContentLoaded", function() {
    document.getElementById("enviar-button").addEventListener("click", function() { 
        const FormData = CollectData();
        EnviarDados(FormData);
    });
});

//definir data de hoje no campo data do pedido
var today = new Date();
var dd = String(today.getDate()).padStart(2, '0');
var mm = String(today.getMonth() + 1).padStart(2, '0'); //January is 0!
var yyyy = today.getFullYear();

today = yyyy + '-' + mm + '-' + dd;
document.getElementById("data-do-pedido").value = today;

function CollectData(){

    const season = document.getElementById("season").value;
    const dataDoPedido = document.getElementById("data-do-pedido").value;
    const pilotoSolicitante = document.getElementById("piloto-solicitante").value;
    const pilotoAcusado = document.getElementById("piloto-acusado").value;
    const simulador = document.getElementById("simulador").value;
    const etapa = document.getElementById("etapa").value;
    const bateria = document.getElementById("bateria").value;
    const minutoDoIncidente = document.getElementById("minuto-do-incidente").value;
    const linkDaGravacao = document.getElementById("link-da-gravacao").value;
    const contexto = document.getElementById("contexto").value;
    
    //Criando um objeto JSON com os dados a serem enviados ao servidor
    const FormData = {
        "Season": season,
        "data-do-pedido": dataDoPedido,
        "piloto-solicitante": pilotoSolicitante,
        "piloto-acusado": pilotoAcusado,
        "simulador": simulador,
        "etapa": etapa,
        "bateria": bateria,
        "minuto-do-incidente": minutoDoIncidente,
        "link-da-gravacao": linkDaGravacao,
        "contexto": contexto
    };

    return FormData;
}

function EnviarDados(FormData){

    //Fazendo uma solicitação POST para a rota do servidor Crow
    fetch("/analysis_request", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(FormData)
    }).then(response => {
        if (response.status === 200) {
            //Sucesso: Os dados foram enviados com sucesso
            alert("[analysis_request] Dados enviados com sucesso!");
        } else {
            //Trate erros de acordo com a resposta do servidor
            alert("[analysis_request] Erro ao enviar dados.");
        }
    }).catch(error => {
        //lidar com erros de conexão
        alert("[analysis_request] Erro de conexão. " + error);
    });
}

/*
document.getElementById('enviar-button').addEventListener('click', function() {
    const FormData = CollectData();
    Notificar_Discord(FormData);
});

function Notificar_Discord(FormData) {

    // Fazer uma requisição usando fetch
    fetch('/analysis-post-receiver', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(FormData),
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Erro na requisição::analysis_request');
        }
        console.log('Requisição bem-sucedida');
        // Recarregar a página ou atualizar os conteúdos conforme necessário
        //location.reload();
    })
    .catch(error => {
        console.error(error);
    });

};
*/