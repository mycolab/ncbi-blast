import datetime
from connexion import NoContent
from flask import request
from flask.views import MethodView


class SpecimenView(MethodView):
    """ Create Pets service
    """
    method_decorators = []
    specimens = {}

    def post(self):
        body = request.json
        name = body.get("name")
        tag = body.get("tag")
        count = len(self.specimens)
        specimen = {
            'id': count + 1,
            'tag': tag,
            'name': name,
            'last_updated': datetime.datetime.now()
        }
        self.specimens[specimen['id']] = specimen
        return specimen, 201

    def put(self, specimen_id):
        body = request.json
        name = body["name"]
        tag = body.get("tag")
        id_ = int(specimen_id)
        pet = self.pets.get(specimen_id, {"id": id_})
        pet["name"] = name
        pet["tag"] = tag
        pet['last_updated'] = datetime.datetime.now()
        self.pets[id_] = pet
        return self.pets[id_], 201

    def delete(self, specimen_id):
        id_ = int(specimen_id)
        if self.pets.get(id_) is None:
            return NoContent, 404
        del self.pets[id_]
        return NoContent, 204

    def get(self, specimen_id):
        id_ = int(specimen_id)
        if self.pets.get(id_) is None:
            return NoContent, 404
        return self.pets[id_]

    def search(self, limit=100):
        # NOTE: we need to wrap it with list for Python 3 as dict_values is not JSON serializable
        return list(self.pets.values())[0:limit]


#
#
# def add_specimen(*args, **kwargs):
#     return kwargs['body'], 200
#
#
# def update_specimen(*args, **kwargs):
#     return {'id': '1234'}, 200
#
#
# def get_specimen(*args, **kwargs):
#     return {'id': '1234'}, 200
#
#
# def delete_specimen(*args, **kwargs):
#     return 204

