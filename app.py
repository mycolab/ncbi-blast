import connexion
# from connexion.resolver import RestyResolver
from connexion.resolver import MethodViewResolver

app = connexion.FlaskApp(__name__)
app.add_api('spec/specimen.yaml', resolver=MethodViewResolver('api'))
app.run(port=8080)
