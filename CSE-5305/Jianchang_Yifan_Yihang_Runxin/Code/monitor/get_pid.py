from kubernetes import client, config
import base64

def list_pods_in_namespace(namespace):
    config.load_kube_config()
    v1 = client.CoreV1Api()
    return v1.list_namespaced_pod(namespace)

def get_pids_in_pod(pod_name, namespace):
    v1 = client.CoreV1Api()

    try:
        exec_command = ['/bin/sh', '-c', 'ps -e']
        resp = v1.connect_get_namespaced_pod_exec(pod_name, namespace,
                                                  command=exec_command,
                                                  stderr=True, stdin=False,
                                                  stdout=True, tty=False)
        return resp
    except client.rest.ApiException as e:
        print(f"Exception when executing command in pod {pod_name}: {e}")

namespace = 'namespace'
pods = list_pods_in_namespace(namespace)

for pod in pods.items:
    get_pids_in_pod(pod.metadata.name, namespace)
