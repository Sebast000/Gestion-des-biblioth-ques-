#include <wx/wx.h>
#include <sqlite3.h>
#include <ctime>
#include <sstream>

// Déclarations des classes principales
class BibliothequeFrame : public wxFrame
{
public:
    BibliothequeFrame(const wxString& title);

private:
    sqlite3* db;

    // Widgets pour l'interface
    wxTextCtrl* txtNom;
    wxTextCtrl* txtId;
    wxTextCtrl* txtDateAdhesion;
    wxTextCtrl* txtLivreCode;
    wxTextCtrl* txtResultat;

    wxButton* btnAjouterAbonne;
    wxButton* btnConsulterHistorique;
    wxButton* btnMettreAJour;
    wxButton* btnAfficherRetards;
    wxButton* btnAjouterEmprunt;

    // Méthodes principales
    void InitialiserBaseDeDonnees();
    void AjouterAbonne(const wxString& nom, const wxString& id, const wxString& dateAdhesion);
    void AjouterEmprunt(const wxString& idAbonne, const wxString& codeLivre);
    void ConsulterHistorique(const wxString& idAbonne);
    void MettreAJourAbonne(const wxString& idAbonne, const wxString& nouveauNom);
    void AfficherRetards();
    void AfficherRetards(const wxString& query);

    // Gestion des événements
    void OnAjouterAbonne(wxCommandEvent& event);
    void OnAjouterEmprunt(wxCommandEvent& event);
    void OnConsulterHistorique(wxCommandEvent& event);
    void OnMettreAJour(wxCommandEvent& event);
    void OnAfficherRetards(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

class BibliothequeApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Événements liés aux boutons
wxBEGIN_EVENT_TABLE(BibliothequeFrame, wxFrame)
    EVT_BUTTON(1001, BibliothequeFrame::OnAjouterAbonne)
    EVT_BUTTON(1002, BibliothequeFrame::OnAjouterEmprunt)
    EVT_BUTTON(1003, BibliothequeFrame::OnConsulterHistorique)
    EVT_BUTTON(1004, BibliothequeFrame::OnMettreAJour)
    EVT_BUTTON(1005, BibliothequeFrame::OnAfficherRetards)
wxEND_EVENT_TABLE()

// Implémentation de BibliothequeFrame
BibliothequeFrame::BibliothequeFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 600))
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Champs de saisie
    txtNom = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    txtId = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    txtDateAdhesion = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    txtLivreCode = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    txtResultat = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(400, 200), wxTE_MULTILINE | wxTE_READONLY);

    // Boutons
    btnAjouterAbonne = new wxButton(this, 1001, "Ajouter Abonne");
    btnAjouterEmprunt = new wxButton(this, 1002, "Ajouter Emprunt");
    btnConsulterHistorique = new wxButton(this, 1003, "Consulter Historique");
    btnMettreAJour = new wxButton(this, 1004, "Mettre a Jour Abonne");
    btnAfficherRetards = new wxButton(this, 1005, "Afficher Retards");

    // Mise en page
    sizer->Add(new wxStaticText(this, wxID_ANY, "Nom abonne :"), 0, wxALL, 5);
    sizer->Add(txtNom, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(this, wxID_ANY, "ID abonne :"), 0, wxALL, 5);
    sizer->Add(txtId, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Date adhesion (YYYY-MM-DD) :"), 0, wxALL, 5);
    sizer->Add(txtDateAdhesion, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Code du livre :"), 0, wxALL, 5);
    sizer->Add(txtLivreCode, 0, wxALL | wxEXPAND, 5);
    sizer->Add(btnAjouterAbonne, 0, wxALL | wxEXPAND, 5);
    sizer->Add(btnAjouterEmprunt, 0, wxALL | wxEXPAND, 5);
    sizer->Add(btnConsulterHistorique, 0, wxALL | wxEXPAND, 5);
    sizer->Add(btnMettreAJour, 0, wxALL | wxEXPAND, 5);
    sizer->Add(btnAfficherRetards, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Resultat :"), 0, wxALL, 5);
    sizer->Add(txtResultat, 1, wxALL | wxEXPAND, 5);

    SetSizerAndFit(sizer);

    // Initialisation de la base de données
    InitialiserBaseDeDonnees();
}

void BibliothequeFrame::InitialiserBaseDeDonnees()
{
    if (sqlite3_open("bibliotheque.db", &db) != SQLITE_OK)
    {
        wxMessageBox("Impossible douvrir la base de donnees !");
        Close();
        return;
    }

    const char* tableQueries =
        "CREATE TABLE IF NOT EXISTS abonnes (" 
        "id TEXT PRIMARY KEY, "
        "nom TEXT NOT NULL, "
        "date_adhesion TEXT NOT NULL); "
        "CREATE TABLE IF NOT EXISTS emprunts (" 
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "abonne_id TEXT NOT NULL, "
        "code_livre TEXT NOT NULL, "
        "date_emprunt TEXT NOT NULL, "
        "date_retour TEXT, "
        "FOREIGN KEY (abonne_id) REFERENCES abonnes (id));";

    sqlite3_exec(db, tableQueries, nullptr, nullptr, nullptr);
}

void BibliothequeFrame::AjouterAbonne(const wxString& nom, const wxString& id, const wxString& dateAdhesion)
{
    wxString query = wxString::Format("INSERT INTO abonnes (id, nom, date_adhesion) VALUES ('%s', '%s', '%s');", id, nom, dateAdhesion);
    if (sqlite3_exec(db, query.mb_str(), nullptr, nullptr, nullptr) == SQLITE_OK)
    {
        wxMessageBox("Abonne ajouter avec succes !");
    }
    else
    {
        wxMessageBox("Erreur lors de ajout de abonne.");
    }
}

void BibliothequeFrame::AjouterEmprunt(const wxString& idAbonne, const wxString& codeLivre)
{
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char date[11];
    strftime(date, sizeof(date), "%Y-%m-%d", now);

    wxString query = wxString::Format("INSERT INTO emprunts (abonne_id, code_livre, date_emprunt) VALUES ('%s', '%s', '%s');", idAbonne, codeLivre, date);
    if (sqlite3_exec(db, query.mb_str(), nullptr, nullptr, nullptr) == SQLITE_OK)
    {
        wxMessageBox("Emprunteur ajouter avec succès !");
    }
    else
    {
        wxMessageBox("Erreur lors de ajout de emprunteur.");
    }
}

void BibliothequeFrame::ConsulterHistorique(const wxString& idAbonne)
{
    wxString query = wxString::Format("SELECT code_livre, date_emprunt, date_retour FROM emprunts WHERE abonne_id = '%s';", idAbonne);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.mb_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        wxString result;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const char* livre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* emprunt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* retour = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            result += wxString::Format("Livre : %s, Emprunter le : %s, Retour le : %s\n", livre, emprunt, retour ? retour : "Pas encore retourné");
        }
        txtResultat->SetValue(result);
        sqlite3_finalize(stmt);
    }
}

void BibliothequeFrame::MettreAJourAbonne(const wxString& idAbonne, const wxString& nouveauNom)
{
    wxString query = wxString::Format("UPDATE abonnes SET nom = '%s' WHERE id = '%s';", nouveauNom, idAbonne);
    if (sqlite3_exec(db, query.mb_str(), nullptr, nullptr, nullptr) == SQLITE_OK)
    {
        wxMessageBox("Informations de abonne mises à jour.");
    }
    else
    {
        wxMessageBox("Erreur lors de la mise à jour.");
    }
}

void BibliothequeFrame::AfficherRetards()
{
    const char* query = "SELECT abonne_id, code_livre, date_emprunt FROM emprunts WHERE date_retour IS NULL;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK)
    {
        wxString result;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const char* abonne_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* code_livre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* date_emprunt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            result += wxString::Format("Abonné ID: %s, Livre: %s, Emprunter le: %s\n", abonne_id, code_livre, date_emprunt);
        }
        txtResultat->SetValue(result);
        sqlite3_finalize(stmt);
    }
}

// Implémentation des gestionnaires d'événements
void BibliothequeFrame::OnAjouterAbonne(wxCommandEvent& event)
{
    AjouterAbonne(txtNom->GetValue(), txtId->GetValue(), txtDateAdhesion->GetValue());
}

void BibliothequeFrame::OnAjouterEmprunt(wxCommandEvent& event)
{
    AjouterEmprunt(txtId->GetValue(), txtLivreCode->GetValue());
}

void BibliothequeFrame::OnConsulterHistorique(wxCommandEvent& event)
{
    ConsulterHistorique(txtId->GetValue());
}

void BibliothequeFrame::OnMettreAJour(wxCommandEvent& event)
{
    MettreAJourAbonne(txtId->GetValue(), txtNom->GetValue());
}

void BibliothequeFrame::OnAfficherRetards(wxCommandEvent& event)
{
    AfficherRetards();
}

// Fonction d'initialisation de l'application wxWidgets
bool BibliothequeApp::OnInit()
{
    BibliothequeFrame* frame = new BibliothequeFrame("Gestion de Bibliotheque");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(BibliothequeApp);
