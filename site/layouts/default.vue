<template>
    <v-app dark>
        <v-content>
            <v-container fluid>
                <nuxt/>
            </v-container>
        </v-content>
        <v-footer class="pa-4">
            <v-layout fill-height align-center justify-center>
                <span>
                    <span class="copyleft">&copy;</span> <a 
                        href="https://alkatrazstudio.net" rel="noreferrer" target="_blank">Alkatraz Studio</a>, 2019.
                </span>
                <span class="ml-1 mr-1">{{ $t('PAGE_LICENSE') }}</span>
                <a href="https://creativecommons.org/licenses/by-sa/4.0/" rel="noreferrer" target="_blank" class="cc-link">
                    <v-img max-height="100%" contain src="~/assets/img/by-sa.svg" alt="CC BY-SA 4.0"/>
                </a>
            </v-layout>
        </v-footer>
        <lang-selector/>
    </v-app>
</template>

<script>
import '@mdi/font/css/materialdesignicons.css'

import LangSelector from '~/components/langSelector'

export default {
    components: {
        LangSelector
    }
}
</script>

<style lang="scss">
#app {
    a {
        color: white;
    }
}
</style>

<style scoped lang="scss">
.container {
    max-width: 1185px!important;
}

.v-footer {
    text-align: center;
}

.cc-link {
    width: 100px;
    display: inline-block;
}

.copyleft {
    transform: scale(-1, 1);
    display: inline-block;
}
</style>
