//coletar o prefixo da temporada atual
let actualSeasonElement = document.querySelector('.tabela_temporada[id^="T"]');
var actualSeason = actualSeasonElement ? actualSeasonElement.id : '';
console.log("first -> actualSeason: " + actualSeason);

function SeasonSelected(event) {
    let selectedOption = event.target.value;
    console.log("[SeasonSelected] Selected option: " + selectedOption);
    toggleSeason(selectedOption);
}

function TableSelected(event) {
    let selectedOption = event.target.value;
    console.log("[TableSelected] Selected option: " + selectedOption);
    toggleTable(event.target.value);
}

function hideAllSeasons(){

    // Encontrar todas as temporadas
    let todasTemporadas = document.querySelectorAll('.tabela_temporada');
        
    todasTemporadas.forEach(function (temporada) {

        // Verificar cada item dentro da temporada
        let itensTemporada = temporada.querySelectorAll('option');

        itensTemporada.forEach(function (item) {

            // Verificar se o valor do item começa com o prefixo
            if (item.value.startsWith(prefix)) {
                // Se começar com o prefixo, mostrar o item
                item.style.display = 'block'; // Ou 'table-row' dependendo do tipo de elemento
            }
            else {
                // Se não começar com o prefixo, ocultar o item
                item.style.display = 'none';
            }

        });

    });
    
}

function hideAllTables(){

    // Encontrar todas as tabelas
    let todasTabelas = document.querySelectorAll('.tabela_etapa');
        
    todasTabelas.forEach(function(tabela) {
        // Ocultar todas as tabelas
        tabela.style.display = 'none';
    });

}

function ShowFirstTableOfSeason(prefix){

    let selectedOption = [...querySelectorAll('.table_selector option[value^="' + prefix + '"]')];
    
    selectedOption.forEach(element => {
        console.log("element: ", element);
    });

}

function toggleSeason(prefix) {

    console.log("entered toggleSeason, prefix: " + prefix);

    hideAllSeasons();
    hideAllTables();
    ShowOnlySeasonTables(prefix);

    // Encontrar a tabela com o ID correspondente ao prefixo
    let temporadaSelecionada = document.querySelector('.tabela_temporada[id^="' + prefix + '"]');

    console.log("prefix: " + prefix);
    console.log("temporadaSelecionada: ", temporadaSelecionada);

    //mudar o titulo da temporada
    let SeasonTitle = document.getElementsByClassName('table_title')[0];
    console.log("SeasonTitle: ", SeasonTitle);
    SeasonTitle.textContent = "Tabela de Classificação " + prefix;

    // Exibir a tabela selecionada
    if (temporadaSelecionada) {
        temporadaSelecionada.style.display = 'table';
    }
}

function toggleTable(prefix) {

    console.log("entered toggleTable");

    hideAllTables();

    // Encontrar a tabela com o ID correspondente ao prefixo
    var tabelaSelecionada = document.querySelector('.tabela_etapa[id^="' + prefix + '"]');

    console.log("prefix: " + prefix);

    // Exibir a tabela selecionada
    if (tabelaSelecionada) {
        tabelaSelecionada.style.display = 'table';
    }
}

function ShowOnlySeasonTables(prefix) {


    console.log("entered ShowOnlySeasonTables with prefix: " + prefix);
    // Seleciona todos os elementos cujo valor começa com o prefixo usando querySelectorAll
    var selectedOptions = document.querySelectorAll('select[name="table_selector"] option[value^="' + prefix + '"]');

    //contrario do selectedOptions
    var notSelectedOptions = document.querySelectorAll('select[name="table_selector"] option:not([value^="' + prefix + '"])');
    
    console.log("selectedOptions length: ", selectedOptions.length);
    
    var prefix;
    // Mostra todos os elementos selecionados
    selectedOptions.forEach(function(option, index) {

        console.log("[SelectedOptions]: ", option.value);
        option.style.display = 'block';

        if(index === 0){
            console.log("index 0::firstSelectedOption: ", option.value);
            prefix = option.value;
        }

    });

    toggleTable(prefix);

    //TableSelected(firstSelectedOption);

    console.log("notSelectedOptions length: ", notSelectedOptions.length);

    // Oculta todos os elementos selecionados
    notSelectedOptions.forEach(function(option) {
        console.log("[notSelectedOptions]: ", option.value);
        option.style.display = 'none';
    });

}

document.addEventListener("DOMContentLoaded", function() {

    console.log("-> entered DOMContentLoaded");
    
    var driver_class_cells = document.querySelectorAll('.driver_class');

    driver_class_cells.forEach(function(cell) {

        var texto = cell.textContent.trim();

        // Definir a cor com base na palavra
        if (texto === 'PRO') {
            cell.classList.add('driver_class_PRO');
        } else if (texto === 'SILVER') {
            cell.classList.add('driver_class_SILVER');
        } else if (texto === 'BRONZE') {
            cell.classList.add('driver_class_BRONZE');
        }
    });

    var driver_punishment_cells = document.querySelectorAll('.driver_punishment');

    driver_punishment_cells.forEach(function(cell) {

        var texto = cell.textContent.trim();

        // Definir a cor com base na palavra
        if (texto != '0') {
            cell.classList.add('driver_punishment_red');
        }
    });

    var driver_upd_cells = document.querySelectorAll('.driver_upd');

    driver_upd_cells.forEach(function(cell) {

        var texto = cell.textContent.trim();

        // Definir a cor com base na palavra
        if (texto === '▲') {
            cell.classList.add('driver_upd_green');
        }
        else if (texto === '►') {
            cell.classList.add('driver_upd_yellow');
        }
        else if (texto === '▼') {
            cell.classList.add('driver_upd_red');
        }
    });

    toggleSeason(actualSeason);
    toggleTable(actualSeason);
});