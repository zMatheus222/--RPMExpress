let canSendData = true;
let canAddEvent = false;

// Adicione uma variável para armazenar os ouvintes do enviarBtn
//let enviarBtnListeners = [];

function openModal(button) {    

    console.log("entered on -> openModal(button)");

    const modal = document.getElementById('myModal');
    const additionalDetailsDiv = document.getElementById('additional-details');

    // Limpa o conteúdo anterior
    additionalDetailsDiv.innerHTML = '';

    // Adiciona os detalhes relevantes
    const mainRow = button.closest('.main-row');
    const detailsRow = mainRow.nextElementSibling;
    const detailsCells = detailsRow.querySelectorAll('.details-container div');

    console.log("entered on -> openModal(button) -> detailsCells: ", detailsCells);

    detailsCells.forEach(cell => {
        const detail = document.createElement('div');
        detail.innerHTML = cell.innerHTML;
        additionalDetailsDiv.appendChild(detail);
    });

    // Adiciona o conteúdo do botão clicado a variavel
    const resolvedButtonContent = button.innerText;

    //adiciona a id da analise
    const AnalysisId = parseInt(mainRow.querySelector('td:first-child').textContent, 10);

    //adiciona a Season da table:
    const Season = mainRow.closest('table').id;

    // Coletar informações da etapa
    const p_solicitante = mainRow.querySelector('td:nth-child(3)').textContent;
    const p_acusado = mainRow.querySelector('td:nth-child(4)').textContent;
    const etapa = mainRow.querySelector('td:nth-child(6)').textContent;
    const bateria = mainRow.querySelector('td:nth-child(7)').textContent;
    const momento = mainRow.querySelector('td:nth-child(8)').textContent;
    const a_status = mainRow.querySelector('td:nth-child(10)').textContent;

    console.log("pre enviarbtn elemby: ");
    
    var enviarBtn = document.getElementById('enviarBtn');
    // Limpa todos os eventos associados ao elemento enviarBtn

    if(canAddEvent === false){
        console.log("--> Adding EventListener on SendClickFunction.");
        enviarBtn.addEventListener("click", SendClickFunction);
    }

    canAddEvent = true;

    function SendClickFunction() {

        console.log("entered on function -> SendClickFunction (ref enviarBtn)");

        // Verifica se pode enviar dados
        if (!canSendData) {
            console.log("Aguarde antes de enviar novamente.");
            return;
        }

        // Bloqueia o envio por 1 segundo
        canSendData = false;

        // Aguarda 1 segundo antes de permitir outro envio
        setTimeout(() => {
            canSendData = true;
            console.log("Pode enviar novamente.");
        }, 500);

        // Coleta as opções selecionadas
        const situacaoPrincipal = parseInt(document.getElementById('campo1').value, 10);
        const situacaoIntrinseca = parseInt(document.getElementById('campo2').value, 10);
        const reincidencia = document.querySelector('.checkbox').checked;
        //
        const Resume = document.getElementById("resume_field").value;

        // Construa um objeto com as informações
        const data = {
            p_solicitante: p_solicitante,
            p_acusado: p_acusado,
            Season: Season,
            etapa: etapa,
            bateria: bateria,
            momento: momento,
            a_status: a_status,
            AnalysisId: AnalysisId,
            situacaoPrincipal: situacaoPrincipal,
            situacaoIntrinseca: situacaoIntrinseca,
            reincidencia: reincidencia,
            resolvedButtonContent: resolvedButtonContent,
            Resume: Resume
        };

        console.log(data);
        try {
            console.log('in const response: fetched /analysis_update');

            // Envie os dados para a rota desejada usando fetch
            const response = fetch('/analysis_update', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
            });

            
            // Use then para acessar a resposta quando a Promise for resolvida
            response.then(response => {

                //verifica o código de status da resposta
                if(response.status === 202){
                    
                    console.log('Status 202, aguardando...');

                    //aguardar um tempo antes de verificar novamente o status
                    return new Promise(resolve => setTimeout(resolve,500));
                }
                else if (response.status === 200){
                    //se o status for 200, transforme o corpo da resposta em JSON
                    return response.json();
                }
                else {
                    //Se o status não for 202 nem 200 lança um erro com o codigo de status
                    throw new Error('Erro na requisição: ${response.status}');
                }

            }).then(data => {
                
                if(data){

                    //se houver dados exibe a mensagem de sucesso
                    console.log('Sucesso', data);

                }
                
                //fechar o modal
                closeModal();

                //recarregar pagina
                location.reload();
                
            });

            console.log('passed if');

        
        } catch (error) {
            console.error('Erro:', error);
            // Aqui você pode lidar com erros de envio
        }
    }

    // Exibe o modal

    modal.style.display = 'block';
}

function openModal_aplicar_punicao(){

    console.log("entered on -> openModal_aplicar_punicao(button)");
    const modal = document.getElementById('myModal_aplicar_punicao');

    console.log("canAddEvent: ", canAddEvent);
    
    var enviarBtn_aplicar_punicao = document.getElementById('enviarBtn_aplicar_punicao');

    if(canAddEvent === false){
        console.log("--> Adding EventListener on SendClickFunction.");
        enviarBtn_aplicar_punicao.addEventListener("click", SendClickFunction);
    }

    canAddEvent = true;

    function SendClickFunction() {

        console.log("entered on function -> SendClickFunction (ref enviarBtn_aplicar_punicao)");

        // Verifica se pode enviar dados
        if (!canSendData) {
            console.log("Aguarde antes de enviar novamente.");
            return;
        }

        // Bloqueia o envio por 1 segundo
        canSendData = false;

        // Aguarda 1 segundo antes de permitir outro envio
        setTimeout(() => {
            canSendData = true;
            console.log("Pode enviar novamente.");
        }, 500);

        // Coleta as opções selecionadas
        const Temp_Race_Bat = document.getElementById('campo_temp').value;
        const pilotoPunido = document.getElementById('campo0_aplicar_punicao').value;
        const situacaoPrincipal = document.getElementById('campo1_aplicar_punicao').value;
        const situacaoIntrinseca = document.getElementById('campo2_aplicar_punicao').value;
        const reincidencia = document.querySelector('.checkbox').checked;

        //
        // Construa um objeto com as informações
        const data = {
            piloto_punido: pilotoPunido,
            campo_temp: Temp_Race_Bat,
            situacaoPrincipal: situacaoPrincipal,
            situacaoIntrinseca: situacaoIntrinseca,
            reincidencia: reincidencia
        };

        console.log("openModal_aplicar_punicao_data: ", data);

        try {
            console.log('in const response: fetched /punishment_apply');

            // Envie os dados para a rota desejada usando fetch
            const response = fetch('/punishment_apply', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
            });

            
            // Use then para acessar a resposta quando a Promise for resolvida
            response.then(response => {

                //verifica o código de status da resposta
                if(response.status === 202){
                    
                    console.log('Status 202, aguardando...');

                    //aguardar um tempo antes de verificar novamente o status
                    return new Promise(resolve => setTimeout(resolve,500));
                }
                else if (response.status === 200){
                    //se o status for 200, transforme o corpo da resposta em JSON
                    return response.json();
                }
                else {
                    //Se o status não for 202 nem 200 lança um erro com o codigo de status
                    throw new Error('Erro na requisição: ${response.status}');
                }

            }).then(data => {
                
                if(data){

                    //se houver dados exibe a mensagem de sucesso
                    console.log('Sucesso', data);

                }
                
                //fechar o modal
                closeModal();

                //recarregar pagina
                location.reload();
                
            });

            console.log('passed if');
        
        } catch (error) {
            console.error('Erro:', error);
            // Aqui você pode lidar com erros de envio
        }
    }

    modal.style.display = 'block';


}

function closeModal() {
    document.getElementById('myModal').style.display = 'none';
    document.getElementById('myModal_aplicar_punicao').style.display = 'none';
}

const mainRows = document.querySelectorAll('.main-row');
mainRows.forEach(mainRow => {

    //botão para abrir o modal de resolução da analise
    const resolvedButton = mainRow.querySelector('.btn-verde, .btn-amarelo');
    resolvedButton.addEventListener('click', function (event) {
        event.stopPropagation(); // Impede que o clique no botão propague para a linha principal
        openModal(this);
    });


});

function toggleSeasons(season) {
    var dropdown = document.getElementById('seasonDropdown');
    dropdown.style.display = (dropdown.style.display === 'block') ? 'none' : 'block';

    showSeason(season);
}

function showSeason(season) {
    var tables = document.querySelectorAll('table');
    tables.forEach(function (table) {
        table.style.display = 'none';
    });

    var selectedTable = document.getElementById(season);
    if (selectedTable) {
        selectedTable.style.display = 'table'; // ou 'block' se preferir
    }
}

window.onload = function() {

    console.log("entered on function window.onload");

    //Seleciona todas as tabelas:
    var tables = document.getElementsByTagName("table");

    //Oculta todas as tabelas, exceto a primeira
    for(var i = 1; i < tables.length; i++){
        tables[i].style.display = "none";
    }
}

document.addEventListener('DOMContentLoaded', function () {

    //botão de aplicar punição
    var ApplyPunishiment = document.querySelector('.btn-apply-punishiment');

    console.log("entered on ApplyPunishiment", ApplyPunishiment);

    ApplyPunishiment.addEventListener('click', function (event) {
        event.stopPropagation(); // Impede que o clique no botão propague para a linha principal
        openModal_aplicar_punicao(this);
    });

});