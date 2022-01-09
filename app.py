import connexion

app = connexion.FlaskApp(__name__, specification_dir='openapi/')
app.add_api('specimen.yaml')
app.run(port=8181)
