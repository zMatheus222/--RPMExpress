function changeDivVisibility(containerId) {
    // Obtém o elemento div pelo ID
    var container = document.getElementById(containerId);
    
    // Altera a visibilidade do div alvo
    container.style.display = 'block';
    
    // Exibe a div da tabela-container
    var mainContainer = document.querySelector('.table-container');
    mainContainer.style.display = 'block';

    // Oculta os outros divs
    var allContainers = document.querySelectorAll('.table-container');
    for (var i = 0; i < allContainers.length; i++) {
        if (allContainers[i].id !== containerId && allContainers[i] !== mainContainer) {
            allContainers[i].style.display = 'none';
        }
    }
}