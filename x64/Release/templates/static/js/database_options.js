AAUpdateBtn.addEventListener('click', function() {

    let sendtype = "update_analysis_show";
    const data = {sendtype: sendtype};

    const response = fetch('/menu', {
        method: 'POST',
        headers: {'Content-Type': 'application/json',},
        body: JSON.stringify(data),
    });

    if (response.status === 200) {
        const responseData = response.json();
        console.log('Sucesso:', responseData);
    }

})

ResultsUpdateBtn.addEventListener('click', function() {

    let sendtype = "update_results_table";
    const data = {sendtype: sendtype};
    
    const response = fetch('/menu', {
        method: 'POST',
        headers: {'Content-Type': 'application/json',},
        body: JSON.stringify(data),
    });

    if (response.status === 200) {
        const responseData = response.json();
        console.log('Sucesso:', responseData);
    }

})

QueryBtn.addEventListener('click', function() {

    let sendtype = "realize_query";
    const data = {sendtype: sendtype};
    
    const response = fetch('/menu', {
        method: 'POST',
        headers: {'Content-Type': 'application/json',},
        body: JSON.stringify(data),
    });

    if (response.status === 200) {
        const responseData = response.json();
        console.log('Sucesso:', responseData);
    }

})