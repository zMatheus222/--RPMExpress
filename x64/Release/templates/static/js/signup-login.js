// Função para alternar entre o formulário de login e registro
function toggleForms() {
    var loginForm = document.getElementById('loginForm');
    var registroForm = document.getElementById('registroForm');
    var cadastroButton = document.getElementById('cadastroButton');

    if (loginForm.style.display === '' || loginForm.style.display === 'block') {
        loginForm.style.display = 'none';
        registroForm.style.display = 'block';
        cadastroButton.style.backgroundColor = 'transparent';
        cadastroButton.style.color = '#2b23c4';
        cadastroButton.innerText = 'Login';
    } else {
        loginForm.style.display = 'block';
        registroForm.style.display = 'none';
        cadastroButton.style.backgroundColor = '#2b23c4';
        cadastroButton.style.color = 'white';
        cadastroButton.innerText = 'Cadastre-se';
    }
}

// Adiciona um ouvinte de evento ao botão de cadastro
document.getElementById('cadastroButton').addEventListener('click', toggleForms);