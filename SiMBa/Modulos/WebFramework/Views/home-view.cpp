#include "views.h"
#include "app.h"

static Content htmlBody1;
static Content htmlBody2;
static Content htmlBody3;


HomeView::HomeView(App * app, Simba * simba) : View( app, simba ) {

    htmlBody1.Init("  <div class=\"container py-5\">"
        "    <h1 class=\"text-center mb-4\">Panel de Control</h1>"
        ""
        "    <div class=\"row justify-content-center mb-4\">"
        "      <div class=\"col-md-6 text-center\">"
        "        <h5 class=\"text-muted\">Estado Actual:</h5>"
        "        <div class=\"d-flex justify-content-center align-items-center\">"
        "           <p id=\"temperaturaActual\" class=\"fs-4 fw-semibold text-primary\">", &htmlBody2);
    
    htmlBody2.SetNext(&htmlBody3);
    
    htmlBody3.Init("ºC</p>"
        "           <button id=\"btnActualizarTemp\" class=\"btn btn-outline-primary btn-sm ms-3\">Actualizar</button>"
        "        </div>"
        "     </div>"
        "   </div>"
       // ""
       // "    <div class=\"row justify-content-center mb-4\">"
       // "      <div class=\"col-md-6 text-center\">"
       // "        <button id=\"btnOn\" class=\"btn btn-success m-2\">Encender</button>"
       // "        <button id=\"btnOff\" class=\"btn btn-danger m-2\">Apagar</button>"
       // "      </div>"
       // "    </div>"
       // ""
       "<div class=\"row justify-content-center\">"
"  <div class=\"col-md-6\">"
"    <div class=\"form-check form-switch fs-5\">"
"      <input class=\"form-check-input\" type=\"checkbox\" id=\"switch1\">"
"      <label class=\"form-check-label\" for=\"switch1\">Modo Manual</label>"
"    </div>"
""
"    <div class=\"form-check form-switch fs-5 mt-3\">"
"      <input class=\"form-check-input\" type=\"checkbox\" id=\"switch2\" disabled>"
"      <label class=\"form-check-label\" for=\"switch2\">Alarma Activada</label>"
"    </div>"
""
"    <div class=\"form-check form-switch fs-5 mt-3\">"
"      <input class=\"form-check-input\" type=\"checkbox\" id=\"switch3\" disabled>"
"      <label class=\"form-check-label\" for=\"switch3\">Tapa Abierta</label>"
"    </div>"
"  </div>"
"</div>"
""
"<script>"
"  const switch1 = document.getElementById('switch1'); "
"  const switch2 = document.getElementById('switch2'); "
"  const switch3 = document.getElementById('switch3'); "
""
"  switch1.addEventListener('change', (e) => {"
"    const manual = e.target.checked;"
""
"    switch2.disabled = !manual;"
"    switch3.disabled = !manual;"
""
"    console.log(\"Modo Manual: \" + (manual ? \"Activado\" : \"Desactivado\"));"
""
"    fetch(manual ? \"/activatemanual\" : \"/deactivatemanual\", {"
"      method: 'POST',"
"      headers: { 'Content-Type': 'application/json' }"
"    })"
"    .then(response => {"
"      if (response.ok) {"
"        console.log(\"POST enviado correctamente\");"
"      } else {"
"        console.error(\"Error en el POST, código HTTP: \" + response.status);"
"      }"
"    })"
"    .catch(error => {"
"      console.error(\"Error en la petición:\", error);"
"    });"
""
"    if (!manual) {"
"      switch2.checked = false;"
"      switch3.checked = false;"
"    }"
"  });"
""
"  switch2.addEventListener('change', (e) => {"
"    console.log(\"Alarma: \" + (e.target.checked ? \"Activada\" : \"Desactivada\"));"
"    fetch(e.target.checked ? \"/alarmon\" : \"/alarmoff\", { method: \"POST\" });"
"  });"
""
"  switch3.addEventListener('change', (e) => {"
"    console.log(\"Tapa: \" + (e.target.checked ? \"Abierta\" : \"Cerrada\"));"
"    fetch(e.target.checked ? \"/openlid\" : \"/closelid\", { method: \"POST\" });"
"  });"
"    document.getElementById(\"btnActualizarTemp\").addEventListener(\"click\", () => {"
"        console.log(\"Actualizando temperatura...\");"
"        fetch(\"/temp\", { method: \"GET\" })"
"          .then(response => response.json())"
"          .then(data => {"
"            document.getElementById(\"temperaturaActual\").innerText = data.temperature + \"ºC\";"
"          })"
"          .catch(error => {"
"            console.error(\"Error al obtener la temperatura:\", error);"
"          });"
"      });"
"</script>",
        nullptr);
}

size_t HomeView::Render(Action action) {
    size_t length = 0;

    switch (action) {
        case Action::GET:
            printf("ACTION GET\r\n");
            length = RenderGet();
            break;
        default: break;
    } 
    return length; 
}

size_t HomeView::RenderGet() {
    static char temp[17];
    
    _simba->ObtenerTemperatura(temp);
    htmlBody2.SetSection(temp);

    _app->layout.RenderHeader("Simba - Home");
    _app->layout.SetInnerContent(&htmlBody1);
    _app->layout.RenderFooter();
    _app->layout.SetEnd(&htmlBody3);
    _app->layout.RenderHttpHeader(200, ContentType::TEXT_HTML);
    //_app->layout.TotalLength();
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}

size_t HomeView::RenderPost() {
    return RenderGet();
}